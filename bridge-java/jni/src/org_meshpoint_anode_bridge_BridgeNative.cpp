#include "org_meshpoint_anode_bridge_BridgeNative.h"

#ifdef ANDROID
#include <AndroidVM.h>
#endif
#include <Env.h>

/*
 * Class:     org_meshpoint_anode_bridge_BridgeNative
 * Method:    callAsFunction
 * Signature: (JJLjava/lang/Object;[Ljava/lang/Object;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_org_meshpoint_anode_bridge_BridgeNative_callAsFunction
(JNIEnv *, jclass, jlong, jlong, jobject, jobjectArray);

/*
 * Class:     org_meshpoint_anode_bridge_BridgeNative
 * Method:    callAsConstructor
 * Signature: (JJ[Ljava/lang/Object;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_org_meshpoint_anode_bridge_BridgeNative_callAsConstructor
(JNIEnv *, jclass, jlong, jlong, jobjectArray);

/*
 * Class:     org_meshpoint_anode_bridge_BridgeNative
 * Method:    getProperty
 * Signature: (JJLjava/lang/String;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_org_meshpoint_anode_bridge_BridgeNative_getProperty
(JNIEnv *, jclass, jlong, jlong, jstring);

/*
 * Class:     org_meshpoint_anode_bridge_BridgeNative
 * Method:    setProperty
 * Signature: (JJLjava/lang/String;Ljava/lang/Object;)V
 */
JNIEXPORT void JNICALL Java_org_meshpoint_anode_bridge_BridgeNative_setProperty
(JNIEnv *, jclass, jlong, jlong, jstring, jobject);

/*
 * Class:     org_meshpoint_anode_bridge_BridgeNative
 * Method:    deleteProperty
 * Signature: (JJLjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_meshpoint_anode_bridge_BridgeNative_deleteProperty
(JNIEnv *, jclass, jlong, jlong, jstring);

/*
 * Class:     org_meshpoint_anode_bridge_BridgeNative
 * Method:    containsProperty
 * Signature: (JJLjava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_meshpoint_anode_bridge_BridgeNative_containsProperty
(JNIEnv *, jclass, jlong, jlong, jstring);

/*
 * Class:     org_meshpoint_anode_bridge_BridgeNative
 * Method:    properties
 * Signature: (JJ)Ljava/util/Collection;
 */
JNIEXPORT jobject JNICALL Java_org_meshpoint_anode_bridge_BridgeNative_properties
(JNIEnv *, jclass, jlong, jlong);

/*
 * Class:     org_meshpoint_anode_bridge_BridgeNative
 * Method:    getIndexedProperty
 * Signature: (JJI)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_org_meshpoint_anode_bridge_BridgeNative_getIndexedProperty
(JNIEnv *, jclass, jlong, jlong, jint);

/*
 * Class:     org_meshpoint_anode_bridge_BridgeNative
 * Method:    setIndexedProperty
 * Signature: (JJILjava/lang/Object;)V
 */
JNIEXPORT void JNICALL Java_org_meshpoint_anode_bridge_BridgeNative_setIndexedProperty
(JNIEnv *, jclass, jlong, jlong, jint, jobject);

/*
 * Class:     org_meshpoint_anode_bridge_BridgeNative
 * Method:    deleteIndexedProperty
 * Signature: (JJI)V
 */
JNIEXPORT void JNICALL Java_org_meshpoint_anode_bridge_BridgeNative_deleteIndexedProperty
(JNIEnv *, jclass, jlong, jlong, jint);

/*
 * Class:     org_meshpoint_anode_bridge_BridgeNative
 * Method:    containsIndex
 * Signature: (JJI)Z
 */
JNIEXPORT jboolean JNICALL Java_org_meshpoint_anode_bridge_BridgeNative_containsIndex
(JNIEnv *, jclass, jlong, jlong, jint);

/*
 * Class:     org_meshpoint_anode_bridge_BridgeNative
 * Method:    length
 * Signature: (JJ)I
 */
JNIEXPORT jint JNICALL Java_org_meshpoint_anode_bridge_BridgeNative_length
(JNIEnv *, jclass, jlong, jlong);

/*
 * Class:     org_meshpoint_anode_bridge_BridgeNative
 * Method:    invokeJSInterface
 * Signature: (JJJI[Ljava/lang/Object;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_org_meshpoint_anode_bridge_BridgeNative_invokeJSInterface
(JNIEnv *jniEnv, jclass, jlong jEnvHandle, jlong jInstHandle, jlong jInterfaceHandle, jint idx, jobjectArray jArgs) {
  Env *env = (Env *)jEnvHandle;
  Interface *interface = (Interface *)jInterfaceHandle;
  return 0;
}

/*
 * Class:     org_meshpoint_anode_bridge_BridgeNative
 * Method:    getJSInterface
 * Signature: (JJJI)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_org_meshpoint_anode_bridge_BridgeNative_getJSInterface
(JNIEnv *jniEnv, jclass, jlong jEnvHandle, jlong jInstHandle, jlong jInterfaceHandle, jint idx) {
  Env *env = (Env *)jEnvHandle;
  Handle<Object> instHandle = asHandle(jInstHandle);
  Interface *interface = (Interface *)jInterfaceHandle;
  
}

/*
 * Class:     org_meshpoint_anode_bridge_BridgeNative
 * Method:    setJSInterface
 * Signature: (JJJILjava/lang/Object;)V
 */
JNIEXPORT void JNICALL Java_org_meshpoint_anode_bridge_BridgeNative_setJSInterface
(JNIEnv *jniEnv, jclass, jlong jEnvHandle, jlong jInstHandle, jlong jInterfaceHandle, jint idx, jobject jVal) {
  Env *env = (Env *)jEnvHandle;
  Handle<Object> instHandle = asHandle(jInstHandle);
  Interface *interface = (Interface *)jInterfaceHandle;
  
}

/*
 * Class:     org_meshpoint_anode_bridge_BridgeNative
 * Method:    releaseObjectHandle
 * Signature: (JJZ)V
 */
JNIEXPORT void JNICALL Java_org_meshpoint_anode_bridge_BridgeNative_releaseObjectHandle
(JNIEnv *jniEnv, jclass, jlong jEnvHandle, jlong jInstHandle, jboolean isPlatform) {
  Env *env = (Env *)jEnvHandle;
  Handle<Object> instHandle = asHandle(jInstHandle);
  
}

/*
 * Class:     org_meshpoint_anode_bridge_BridgeNative
 * Method:    bindInterface
 * Signature: (JLorg/meshpoint/anode/idl/IDLInterface;IIILjava/lang/Class;Ljava/lang/Class;Ljava/lang/Class;)J
 */
JNIEXPORT jlong JNICALL Java_org_meshpoint_anode_bridge_BridgeNative_bindInterface
(JNIEnv *jniEnv, jclass, jlong jEnvHandle, jobject jInterface, jint jClassId, jint attrCount, jint opCount, jclass jUserStub, jclass jPlatformStub, jclass jDictStub) {
  Env *env = (Env *)jEnvHandle;
  
}

/*
 * Class:     org_meshpoint_anode_bridge_BridgeNative
 * Method:    bindAttribute
 * Signature: (JJIILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_meshpoint_anode_bridge_BridgeNative_bindAttribute
(JNIEnv *jniEnv, jclass, jlong jEnvHandle, jlong jInterfaceHandle, jint attrIdx, jint type, jstring jName) {
  Env *env = (Env *)jEnvHandle;
  Interface *interface = (Interface *)jInterfaceHandle;
}

/*
 * Class:     org_meshpoint_anode_bridge_BridgeNative
 * Method:    bindOperation
 * Signature: (JJIILjava/lang/String;I[I)V
 */
JNIEXPORT void JNICALL Java_org_meshpoint_anode_bridge_BridgeNative_bindOperation
(JNIEnv *jniEnv, jclass, jlong jEnvHandle, jlong jInterfaceHandle, jint opIdx, jint type, jstring jName, jint argCount, jintArray jArgTypes) {
  Env *env = (Env *)jEnvHandle;
  Interface *interface = (Interface *)jInterfaceHandle;
  
}

/*
 * Class:     org_meshpoint_anode_bridge_BridgeNative
 * Method:    releaseInterface
 * Signature: (JJ)V
 */
JNIEXPORT void JNICALL Java_org_meshpoint_anode_bridge_BridgeNative_releaseInterface
(JNIEnv *jniEnv, jclass, jlong jEnvHandle, jlong jInterfaceHandle) {
  Env *env = (Env *)jEnvHandle;
  Interface *interface = (Interface *)jInterfaceHandle;
  
}

/*
 * Class:     org_meshpoint_anode_bridge_BridgeNative
 * Method:    requestEntry
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_org_meshpoint_anode_bridge_BridgeNative_requestEntry
(JNIEnv *jniEnv, jclass, jlong jEnvHandle) {
  Env *env = (Env *)jEnvHandle;

}


/*
 * Class:     org_meshpoint_anode_bridge_BridgeNative
 * Method:    setContext
 * Signature: (Ljava/lang/Object;)V
 */
JNIEXPORT void JNICALL Java_org_meshpoint_anode_bridge_BridgeNative_setContext
  (JNIEnv *jniEnv, jclass, jobject ctx) {
#ifdef ANDROID
	AndroidVM *vm = new AndroidVM(jniEnv, ctx);
	Env::setupEnv(vm);
#endif
}
