#include "org_meshpoint_anode_RuntimeNative.h"

#include <string.h>
#include "node.h"
#define DEBUG
#ifdef DEBUG
# include <android/log.h>
# define DEBUG_TAG "libjninode"
# define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, DEBUG_TAG, __VA_ARGS__)
#else
# define LOGV(...)
#endif

static void freeNativeArgs(jint argc, char **argv) {
	for(int i = 0; i < argc; i++)
		delete[] argv[i + 1];
	delete[] argv;
}

static int getNativeArgs(JNIEnv *jniEnv, jobjectArray jargv, char ***pargv) {
	LOGV("getNativeArgs: ent\n");
  	jint argc = jargv ? jniEnv->GetArrayLength(jargv) : 0;

	/* convert jargv to native */
  	char **argv = new char*[argc + 2];
  	if(!argv) return -1;
  	argv[0] = strdup((char *)"node");
  	if(!argv[0]) { freeNativeArgs(0, argv); return -1; }
  	if(jargv) {
		for(int i = 0; i < argc; i++) {
			jstring jarg = (jstring)jniEnv->GetObjectArrayElement(jargv, i);
			if(!jarg)  { freeNativeArgs(i, argv); return -1; }
			char *argCopy = 0;
			const char *arg = jniEnv->GetStringUTFChars(jarg, 0);
			int argLen = jniEnv->GetStringUTFLength(jarg);
			argCopy = new char[argLen + 1];
			if(!argCopy)  { freeNativeArgs(i, argv); return -1; }
			memcpy(argCopy, arg, argLen);
			argCopy[argLen] = 0;
			jniEnv->ReleaseStringUTFChars(jarg, arg);
			argv[i + 1] = argCopy;
		}
	}
	argv[++argc] = 0;
	*pargv = argv;
	LOGV("getNativeArgs: ret %d\n", argc);
	return argc;
}

/*
 * Class:     org_meshpoint_anode_RuntimeNative
 * Method:    nodeInit
 * Signature: ([Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_meshpoint_anode_RuntimeNative_nodeInit
  (JNIEnv *jniEnv, jclass, jobjectArray jargv) {
	LOGV("Java_org_meshpoint_anode_RuntimeNative_nodeInit: ent\n");
  char **argv;
  int argc;
  if((argc = getNativeArgs(jniEnv, jargv, &argv)) >= 0)
	  node::Initialize(argc, argv);
	LOGV("Java_org_meshpoint_anode_RuntimeNative_nodeInit: ret\n");
}

/*
 * Class:     org_meshpoint_anode_RuntimeNative
 * Method:    nodeDispose
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_org_meshpoint_anode_RuntimeNative_nodeDispose
  (JNIEnv *, jclass) {
	LOGV("Java_org_meshpoint_anode_RuntimeNative_nodeDispose: ent\n");
	node::Dispose();
	LOGV("Java_org_meshpoint_anode_RuntimeNative_nodeDispose: ret\n");
}

/*
 * Class:     org_meshpoint_anode_RuntimeNative
 * Method:    create
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_org_meshpoint_anode_RuntimeNative_create
  (JNIEnv *, jclass) {
	LOGV("Java_org_meshpoint_anode_RuntimeNative_create\n");
  	return (jlong)node::Isolate::New();
}

/*
 * Class:     org_meshpoint_anode_RuntimeNative
 * Method:    start
 * Signature: (J[Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_org_meshpoint_anode_RuntimeNative_start
  (JNIEnv *jniEnv, jclass, jlong handle, jobjectArray jargv) {
	LOGV("Java_org_meshpoint_anode_RuntimeNative_start: ent\n");
	node::Isolate *isolate = reinterpret_cast<node::Isolate *>(handle);
	char **argv;
	int argc;
	if((argc = getNativeArgs(jniEnv, jargv, &argv)) >= 0) {
		int result = isolate->Start(argc, argv);
		freeNativeArgs(argc, argv);
		argc = result;
	}
	LOGV("Java_org_meshpoint_anode_RuntimeNative_start: ret %d\n", argc);
	return argc;
}

/*
 * Class:     org_meshpoint_anode_RuntimeNative
 * Method:    stop
 * Signature: (JI)I
 */
JNIEXPORT jint JNICALL Java_org_meshpoint_anode_RuntimeNative_stop
  (JNIEnv *, jclass, jlong handle, jint signum) {
	node::Isolate *isolate = reinterpret_cast<node::Isolate *>(handle);
	LOGV("Java_org_meshpoint_anode_RuntimeNative_start: ent\n");
  	int result = isolate->Stop(signum);
  	LOGV("Java_org_meshpoint_anode_RuntimeNative_start: ret %d\n", result);
  	return result;
}

/*
 * Class:     org_meshpoint_anode_RuntimeNative
 * Method:    isolateDispose
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_org_meshpoint_anode_RuntimeNative_isolateDispose
  (JNIEnv *, jclass, jlong handle) {
	node::Isolate *isolate = reinterpret_cast<node::Isolate *>(handle);
	LOGV("Java_org_meshpoint_anode_RuntimeNative_isolateDispose: ent\n");
  	isolate->Dispose();
  	LOGV("Java_org_meshpoint_anode_RuntimeNative_isolateDispose: ret\n");
}
