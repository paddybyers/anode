#include "org_meshpoint_anode_RuntimeNative.h"

#include <string.h>
#include "node.h"

static void freeNativeArgs(jint argc, char **argv) {
	for(int i = 0; i < argc; i++)
		delete[] argv[i + 1];
	delete[] argv;
}

static int getNativeArgs(JNIEnv *jniEnv, jobjectArray jargv, char ***pargv) {
  	jint argc = jargv ? jniEnv->GetArrayLength(jargv) : 0;
	if(!argc) { *pargv = 0; return 0; }

	/* convert jargv to native */
  	char **argv = new char*[argc + 2];
  	if(!argv) return -1;
  	argv[0] = strdup((char *)"node");
  	if(!argv[0]) { freeNativeArgs(0, argv); return -1; }
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
	argv[++argc] = 0;
	*pargv = argv;
	return argc;
}

/*
 * Class:     org_meshpoint_anode_RuntimeNative
 * Method:    nodeInit
 * Signature: ([Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_meshpoint_anode_RuntimeNative_nodeInit
  (JNIEnv *jniEnv, jclass, jobjectArray jargv) {
  char **argv;
  int argc;
  if((argc = getNativeArgs(jniEnv, jargv, &argv)) >= 0)
	  node::Initialize(argc, argv);
}

/*
 * Class:     org_meshpoint_anode_RuntimeNative
 * Method:    nodeDispose
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_org_meshpoint_anode_RuntimeNative_nodeDispose
  (JNIEnv *, jclass) {
  node::Dispose();
}

/*
 * Class:     org_meshpoint_anode_RuntimeNative
 * Method:    create
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_org_meshpoint_anode_RuntimeNative_create
  (JNIEnv *, jclass) {
  	return (jlong)node::Isolate::New();
}

/*
 * Class:     org_meshpoint_anode_RuntimeNative
 * Method:    start
 * Signature: (J[Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_org_meshpoint_anode_RuntimeNative_start
  (JNIEnv *jniEnv, jclass, jlong handle, jobjectArray jargv) {
	node::Isolate *isolate = reinterpret_cast<node::Isolate *>(handle);
	char **argv;
	int argc;
	if((argc = getNativeArgs(jniEnv, jargv, &argv)) >= 0) {
		int result = isolate->Start(argc, argv);
		freeNativeArgs(argc, argv);
		argc = result;
	}
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
  	return isolate->Stop(signum);
}

/*
 * Class:     org_meshpoint_anode_RuntimeNative
 * Method:    isolateDispose
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_org_meshpoint_anode_RuntimeNative_isolateDispose
  (JNIEnv *, jclass, jlong handle) {
	node::Isolate *isolate = reinterpret_cast<node::Isolate *>(handle);
  	return isolate->Dispose();
}
