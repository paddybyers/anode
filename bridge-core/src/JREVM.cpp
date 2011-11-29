#include "JREVM.h"

#include <pthread.h>

static pthread_mutext_t mtx;

JREVM::JREVM() {
	attach();
}

int JREVM::attach() {
	pthread_mutex_lock(&mtx);
	++attachCount;
	int status = jniJVM->GetEnv((void **)&jniEnv, JNI_VERSION_1_2);
	if(status != JNI_OK) {
		if(status == JNI_EDETACHED) {
			status = jniJVM->AttachCurrentThread((void **)&jniEnv, 0);
		}
	}
	pthread_mutex_unlock(&mtx);
	if(status != JNI_OK) {
		fprintf(stderr, "Fatal error: unable to attach to Java VM\n");
		return ErrorVM;
	}
	return OK;
}

JREVM::~JREVM() {
	if(javaVM) {
		pthread_mutex_lock(&mtx);
		javaVM->DetachCurrentThread();
		int count = --attachCount;
		pthread_mutex_unlock(&mtx);
		result = (status==JNI_OK) ? (int)OK : (int)Error_VM;
		if(!count) {
			javaVM->DestroyJavaVM();
		}
	}
}

int JREVM::static_init(Env *env) {
	attachCount = 0;
	pthread_mutex_init(&mtx, 0);
	return createVM();
}

char *JREVM::buildClasspath(const char *anodeRoot, size_t len) {
	const char *part0 = "-Djava.class.path=";
	const char *part1 = "/bridge-java/bin/:";
	const char *part2 = "/api/bin/";
	char *result = new char[len * 2 + sizeof(part0) + sizeof(part1) + sizeof(part2)];
	if(result) {
		strcpy(result, part0);
		strcat(result, anodeRoot);
		strcat(result, part1);
		strcat(result, anodeRoot);
		strcat(result, part2);
	}
	return result;
}

char *JREVM::buildLibrarypath(const char *anodeRoot, size_t len) {
	const char *part0 = "-Djava.library.path=";
	const char *part1 = "/bridge-java/jni/lib/";
	char *result = new char[len + sizeof(part0) + sizeof(part1)];
	if(result) {
		strcpy(result, part0);
		strcat(result, anodeRoot);
		strcat(result, part1);
	}
	return result;
}

int JREVM::createVM() {
	jint (*jniCreateJavaVM)(JavaVM**, JNIEnv**, JavaVMInitArgs*);
	jint (*jniGetCreatedJavaVMs)(JavaVM**, jsize, jsize*);

#ifdef DARWIN
	jniGetCreatedJavaVMs = &JNI_GetCreatedJavaVMs;
	jniCreateJavaVM = &JNI_CreateJavaVM;
#else
	int result = uv_dlopen(JRE_LIB, &jniLib);
	if(!result) {
		result = uv_dlsym(jniLib, "GetCreatedJavaVMs" (void **)&jniGetCreatedJavaVMs);
		if(!result)
			result = uv_dlsym(jniLib, "CreateJavaVM" (void **)&jniCreateJavaVM);
	}
	if(result) {
		/* fatal */
		fprintf(stderr, "Fatal error: unable to locate JNI entrypoints\n");
		return result;
	}
#endif
	/* attempt to get existing VM */
	jsize vmCount = 0;
	jint status = jniGetCreatedJavaVMs(&javaVM,1,&vmCount);
	if(status != JNI_OK) {
		/* fatal */
		fprintf(stderr, "Fatal error: unable to get created Java VMs\n");
		return ErrorVM;
	}
	if(vmCount > 0)
		return OK;

	/* start a VM for this process */
	JavaVMInitArgs jvmInitargs;
	JavaVMOption options[4];
	jvmInitargs.options            = options;
	jvmInitargs.ignoreUnrecognized = true;
	jvmInitargs.version            = JNI_VERSION_1_2;

	const char *anodeRoot = getenv("ANODE_ROOT");
	if(!anodeRoot) {
		/* fatal */
		fprintf(stderr, "Fatal error: ANODE_ROOT environment variable not found\n");
		return ErrorConfig;
	}
	size_t rootLen = strlen(anodeRoot);
	options[0].optionString = buildClasspath(anodeRoot, rootLen);
	options[0].extraInfo    = 0;
	options[1].optionString = buildLibrarypath(anodeRoot, rootLen);
	options[1].extraInfo    = 0;
	options[2].optionString = (char *)"-Xdebug";
	options[2].extraInfo    = 0;
	options[3].optionString = (char *)"-Xrunjdwp:transport=dt_socket,server=y,address=8000,suspend=n";
	options[3].extraInfo    = 0;

#ifdef DEBUG
	jvmInitargs.nOptions = 4
#else
	jvmInitargs.nOptions = 2
#endif

	int status = jniCreateJavaVM(&javaVM,&jniEnv,&jvmInitargs);
	
	delete[] options[0].optionString;
	delete[] options[1].optionString;

	if(status == JNI_OK)
		return OK;

	/* fatal */
	fprintf(stderr, "Fatal error: unable to create Java VM\n");
	return ErrorVM;
}
