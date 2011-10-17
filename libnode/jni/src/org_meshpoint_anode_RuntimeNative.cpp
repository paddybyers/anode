#include "org_meshpoint_anode_RuntimeNative.h"

#include <string.h>
#include "lib_wrapper.h"

/*
 * Class:     org_meshpoint_anode_RuntimeNative
 * Method:    start
 * Signature: ([Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_org_meshpoint_anode_RuntimeNative_start
  (JNIEnv *jniEnv, jclass, jobjectArray jargv) {
	int result = -1;
	/* convert jargv to native */
  	jint argc = jniEnv->GetArrayLength(jargv);
  	char **argv = new char*[argc + 2];
  	if(argv) {
  		argv[0] = (char *)"node";
		for(int i = 0; i < argc; i++) {
			jstring jarg = (jstring)jniEnv->GetObjectArrayElement(jargv, i);
			char *argCopy = 0;
			if(jarg) {
				const char *arg = jniEnv->GetStringUTFChars(jarg, 0);
				int argLen = jniEnv->GetStringUTFLength(jarg);
				argCopy = new char[argLen + 1];
				if(argCopy) {
					memcpy(argCopy, arg, argLen);
					argCopy[argLen] = 0;
				}
				jniEnv->ReleaseStringUTFChars(jarg, arg);
			}
			argv[i + 1] = argCopy;
		}
		argv[argc + 1] = 0;
		/* make the call */
		result = libnode_Start(argc + 1, argv);
		/* release argv */
		for(int i = 0; i < argc; i++)
			delete[] argv[i + 1];
		delete[] argv;
	}
	return result;
}

/*
 * Class:     org_meshpoint_anode_RuntimeNative
 * Method:    stop
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_org_meshpoint_anode_RuntimeNative_stop
  (JNIEnv *, jclass, jint signum) {
  	return libnode_Stop(signum);
}
