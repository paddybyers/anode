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
(JNIEnv *jniEnv, jclass, jlong /*jEnvHandle*/, jlong jInstHandle, jlong jInterfaceHandle, jint idx, jobjectArray jArgs) {
  Handle<Object> instHandle = asHandle(jInstHandle);
  Interface *interface = (Interface *)jInterfaceHandle;
  jobject jResult = 0;
  int result = interface->UserInvoke(jniEnv, instHandle, idx, jArgs, &jResult);
  if(result != OK)
    LOGV("Unable to get property on user interface: err = %d\n", result);
  return jResult;
}

/*
 * Class:     org_meshpoint_anode_bridge_BridgeNative
 * Method:    getJSInterface
 * Signature: (JJJI)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_org_meshpoint_anode_bridge_BridgeNative_getJSInterface
(JNIEnv *jniEnv, jclass, jlong /*jEnvHandle*/, jlong jInstHandle, jlong jInterfaceHandle, jint idx) {
  Handle<Object> instHandle = asHandle(jInstHandle);
  Interface *interface = (Interface *)jInterfaceHandle;
  jobject jVal = 0;
  int result = interface->UserGet(jniEnv, instHandle, idx, &jVal);
  if(result != OK)
    LOGV("Unable to get property on user interface: err = %d\n", result);
  return jVal;
}

/*
 * Class:     org_meshpoint_anode_bridge_BridgeNative
 * Method:    setJSInterface
 * Signature: (JJJILjava/lang/Object;)V
 */
JNIEXPORT void JNICALL Java_org_meshpoint_anode_bridge_BridgeNative_setJSInterface
(JNIEnv *jniEnv, jclass, jlong /*jEnvHandle*/, jlong jInstHandle, jlong jInterfaceHandle, jint idx, jobject jVal) {
  Handle<Object> instHandle = asHandle(jInstHandle);
  Interface *interface = (Interface *)jInterfaceHandle;
  int result = interface->UserSet(jniEnv, instHandle, idx, jVal);
  if(result != OK) {
    LOGV("Unable to set property on user interface: err = %d\n", result);
  }
}

/*
 * Class:     org_meshpoint_anode_bridge_BridgeNative
 * Method:    releaseObjectHandle
 * Signature: (JJI)V
 */
JNIEXPORT void JNICALL Java_org_meshpoint_anode_bridge_BridgeNative_releaseObjectHandle
(JNIEnv *jniEnv, jclass, jlong jEnvHandle, jlong jInstHandle, jint jClassId) {
  Env *env = (Env *)jEnvHandle;
  Persistent<Object> instHandle = asHandle(jInstHandle);
  env->getConv()->releaseV8Handle(jniEnv, instHandle, jClassId);
}

/*
 * Class:     org_meshpoint_anode_bridge_BridgeNative
 * Method:    bindInterface
 * Signature: (JLorg/meshpoint/anode/idl/IDLInterface;IIILjava/lang/Class;Ljava/lang/Class;Ljava/lang/Class;)J
 */
JNIEXPORT jlong JNICALL Java_org_meshpoint_anode_bridge_BridgeNative_bindInterface
(JNIEnv *jniEnv, jclass, jlong jEnvHandle, jobject jInterface, jint jClassId, jint attrCount, jint opCount, jclass jUserStub, jclass jPlatformStub, jclass jDictStub) {
  Env *env = (Env *)jEnvHandle;
  Interface *interface;
  int result = Interface::Create(jniEnv, env->getConv(), jInterface, jClassId, attrCount, opCount, jUserStub, jPlatformStub, jDictStub, &interface);
  if(result == OK)
    return (jlong)interface;
  LOGV("Unable to create Interface: err = %d\n", result);
  return 0;
}

/*
 * Class:     org_meshpoint_anode_bridge_BridgeNative
 * Method:    bindAttribute
 * Signature: (JJIILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_meshpoint_anode_bridge_BridgeNative_bindAttribute
(JNIEnv *jniEnv, jclass, jlong /*jEnvHandle*/, jlong jInterfaceHandle, jint attrIdx, jint type, jstring jName) {
  Interface *interface = (Interface *)jInterfaceHandle;
  int result = interface->initAttribute(jniEnv, attrIdx, type, jName);
  if(result != OK) {
    LOGV("Unable to init Attribute: err = %d\n", result);
  }
}

/*
 * Class:     org_meshpoint_anode_bridge_BridgeNative
 * Method:    bindOperation
 * Signature: (JJIILjava/lang/String;I[I)V
 */
JNIEXPORT void JNICALL Java_org_meshpoint_anode_bridge_BridgeNative_bindOperation
(JNIEnv *jniEnv, jclass, jlong /*jEnvHandle*/, jlong jInterfaceHandle, jint opIdx, jint type, jstring jName, jint argCount, jintArray jArgTypes) {
  Interface *interface = (Interface *)jInterfaceHandle;
  jint *argTypes = jniEnv->GetIntArrayElements(jArgTypes, 0);
  int result = interface->initOperation(jniEnv, opIdx, type, jName, argCount, argTypes);
  jniEnv->ReleaseIntArrayElements(jArgTypes, argTypes, 0);
  if(result != OK) {
    LOGV("Unable to init Operation: err = %d\n", result);
  }
}

/*
 * Class:     org_meshpoint_anode_bridge_BridgeNative
 * Method:    releaseInterface
 * Signature: (JJ)V
 */
JNIEXPORT void JNICALL Java_org_meshpoint_anode_bridge_BridgeNative_releaseInterface
(JNIEnv *jniEnv, jclass, jlong /*jEnvHandle*/, jlong jInterfaceHandle) {
  Interface *interface = (Interface *)jInterfaceHandle;
  interface->dispose(jniEnv);
  delete interface;
}

/*
 * Class:     org_meshpoint_anode_bridge_BridgeNative
 * Method:    requestEntry
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_org_meshpoint_anode_bridge_BridgeNative_requestEntry
(JNIEnv *jniEnv, jclass, jlong jEnvHandle) {
  Env *env = (Env *)jEnvHandle;
  env->setAsync();
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
