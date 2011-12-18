#include "JREVM.h"

#include "defines.h"
#include <dlfcn.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

static pthread_mutex_t mtx;
static JavaVM *javaVM;
static int attachCount;

#ifndef __APPLE__
static void *jniLib;
#endif

using namespace bridge;

JREVM::JREVM() : VM() {
	attach();
  /* initialisation specific to this VM type */
	jContextClass = (jclass)jniEnv->NewGlobalRef(jniEnv->FindClass("org/meshpoint/anode/bridge/ModuleContext"));
	createContextMethodId = jniEnv->GetMethodID(jContextClass, "<init>", "(Lorg/meshpoint/anode/bridge/Env;Lorg/meshpoint/anode/js/JSObject;)V");
}

int JREVM::attach() {
	pthread_mutex_lock(&mtx);
	++attachCount;
	int status = javaVM->GetEnv((void **)&jniEnv, JNI_VERSION_1_2);
	if(status != JNI_OK) {
		if(status == JNI_EDETACHED) {
			status = javaVM->AttachCurrentThread((void **)&jniEnv, 0);
		}
	}
	pthread_mutex_unlock(&mtx);
	if(status != JNI_OK) {
		LOGV("Fatal error: unable to attach to Java VM\n");
		return ErrorVM;
	}
	return OK;
}

JREVM::~JREVM() {
	if(javaVM) {
    jniEnv->DeleteGlobalRef(jContextClass);
		pthread_mutex_lock(&mtx);
		javaVM->DetachCurrentThread();
		int count = --attachCount;
		pthread_mutex_unlock(&mtx);
		if(!count) {
			javaVM->DestroyJavaVM();
#ifndef __APPLE__
			dlclose(jniLib);
#endif
		}
	}
}

int JREVM::createContext(jobject jEnv, jobject jExports, jobject *jCtx) {
  int result = OK;
	*jCtx = jniEnv->NewObject(jContextClass, createContextMethodId, jEnv, jExports);
  if(jniEnv->ExceptionCheck()) {
		result = ErrorVM;
		jniEnv->ExceptionClear();
	}
  return result;
}

int JREVM::static_init() {
	attachCount = 0;
	pthread_mutex_init(&mtx, 0);
	return createVM();
}

#define LITERAL(text, identifier) const char *str##identifier = text; size_t i##identifier = sizeof(text)-1
#define LAPPEND(dest, literal, offset) memcpy(&dest[offset], str##literal, i##literal); offset += i##literal
#define DAPPEND(dest, str, len, offset) memcpy(&dest[offset], str, len); offset += len

char *JREVM::buildClasspath(const char *anodeRoot, size_t len) {
	LITERAL("-Djava.class.path=", Part0);
	LITERAL("/bridge-java/bin/:", Part1);
  LITERAL("/api/bin/", Part2);
	char *result = new char[len * 2 + iPart0 + iPart1 + iPart2 + 1];
	if(result) {
    size_t tmp = 0;
    LAPPEND(result, Part0, tmp);
    DAPPEND(result, anodeRoot, len, tmp);
    LAPPEND(result, Part1, tmp);
    DAPPEND(result, anodeRoot, len, tmp);
    LAPPEND(result, Part2, tmp);
    result[tmp] = 0;
	}
	return result;
}

char *JREVM::buildLibrarypath(const char *anodeRoot, size_t len) {
	LITERAL("-Djava.library.path=", Part0);
	LITERAL("/bridge-java/jni/lib/", Part1);
	char *result = new char[len + iPart0 + iPart1 + 1];
	if(result) {
    size_t tmp = 0;
    LAPPEND(result, Part0, tmp);
    DAPPEND(result, anodeRoot, len, tmp);
    LAPPEND(result, Part1, tmp);
    result[tmp] = 0;
	}
	return result;
}

int JREVM::createVM() {

#ifdef __APPLE__
	jint (*jniCreateJavaVM)(JavaVM**, JNIEnv**, JavaVMInitArgs*) = (jint (*)(JavaVM**, JNIEnv**, JavaVMInitArgs*))JNI_CreateJavaVM;
	jint (*jniGetCreatedJavaVMs)(JavaVM**, jsize, jsize*) = JNI_GetCreatedJavaVMs;
#else
	jint (*jniCreateJavaVM)(JavaVM**, JNIEnv**, JavaVMInitArgs*);
	jint (*jniGetCreatedJavaVMs)(JavaVM**, jsize, jsize*);
	jniLib = dlopen(JRE_LIB, RTLD_LAZY);
	if(jniLib) {
		jniGetCreatedJavaVMs = (jint (*)(JavaVM**, jsize, jsize*))dlsym(jniLib, "GetCreatedJavaVMs");
		jniCreateJavaVM = (jint (*)(JavaVM**, JNIEnv**, JavaVMInitArgs*))dlsym(jniLib, "CreateJavaVM");
	}
	if(!jniGetCreatedJavaVMs || ! jniCreateJavaVM) {
		/* fatal */
		LOGV("Fatal error: unable to locate JNI entrypoints\n");
		return ErrorVM;
	}
#endif
	/* attempt to get existing VM */
	jsize vmCount = 0;
	jint status = jniGetCreatedJavaVMs(&javaVM,1,&vmCount);
	if(status != JNI_OK) {
		/* fatal */
		LOGV("Fatal error: unable to get created Java VMs\n");
		return ErrorVM;
	}
	if(vmCount > 0)
		return OK;

	/* start a VM for this process */
	JavaVMInitArgs jvmInitargs;
#ifdef DEBUG
	jvmInitargs.nOptions = 4;
#else
	jvmInitargs.nOptions = 2
#endif
  
	JavaVMOption *options = new JavaVMOption[jvmInitargs.nOptions];
	jvmInitargs.options            = options;
	jvmInitargs.ignoreUnrecognized = true;
	jvmInitargs.version            = JNI_VERSION_1_4;

	const char *anodeRoot = getenv("ANODE_ROOT");
	if(!anodeRoot) {
		/* fatal */
		LOGV("Fatal error: ANODE_ROOT environment variable not found\n");
		return ErrorConfig;
	}
	size_t rootLen = strlen(anodeRoot);
	options[0].optionString = buildClasspath(anodeRoot, rootLen);
	options[0].extraInfo    = 0;
	options[1].optionString = buildLibrarypath(anodeRoot, rootLen);
	options[1].extraInfo    = 0;
#ifdef DEBUG
	options[2].optionString = (char *)"-Xdebug";
	options[2].extraInfo    = 0;
	options[3].optionString = (char *)"-Xrunjdwp:transport=dt_socket,server=y,address=8000,suspend=n";
	options[3].extraInfo    = 0;
#endif

  JNIEnv *tmpEmv;
	status = jniCreateJavaVM(&javaVM, &tmpEmv, &jvmInitargs);
	
	delete[] options[0].optionString;
	delete[] options[1].optionString;
  delete[] options;

	if(status == JNI_OK)
		return OK;

	/* fatal */
	LOGV("Fatal error: unable to create Java VM\n");
	return ErrorVM;
}
