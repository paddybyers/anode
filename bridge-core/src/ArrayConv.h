#ifndef BRIDGE_ARRAYCONV_H
#define BRIDGE_ARRAYCONV_H

#include "defines.h"
#include "Conv.h"
#include "Utils.h"

namespace bridge {

class Env;

class ArrayType {
public:
  unsigned int componentType;
  classRefs_s js;
  classRefs_s java;
  jmethodID getLength;
  jmethodID setLength;
  jmethodID getElement;
  jmethodID setElement;
  v8::Persistent<v8::String> sHiddenKey;
  v8::Persistent<v8::String> sClassName;
  v8::Persistent<v8::FunctionTemplate> functionTemplate;
  v8::Persistent<v8::Function> function;
  ArrayType(Env *env, JNIEnv *jniEnv,
            unsigned int componentType,
            const char *ClassName,
            const char *jsCtor,
            const char *javaGetter,
            const char *javaSetter
  );
  ~ArrayType() {}
  int UserNew(JNIEnv *jniEnv, jobject *jVal);
  int PlatformNew(JNIEnv *jniEnv, v8::Handle<v8::Object> *val);
  void dispose(JNIEnv *jniEnv);
private:
  Env *env;
  static v8::Handle<v8::Value> UserLengthGet(v8::Local<v8::String> property, const v8::AccessorInfo& info);
  static void UserLengthSet(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
  static v8::Handle<v8::Value> UserElementGet(uint32_t index, const v8::AccessorInfo& info);
  static v8::Handle<v8::Value> UserElementSet(uint32_t index, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
};

class ArrayConv {
public:
  ArrayConv(Env *env, Conv *conv, JNIEnv *jniEnv);
  void dispose(JNIEnv *jniEnv);
  ~ArrayConv();
  inline v8::Persistent<v8::String> getHiddenKey() {return sArrayHiddenKey;}
  inline v8::Persistent<v8::String> getSLength() {return sLength;}
  int GetArrayType(JNIEnv *jniEnv, classId class_, ArrayType **ref);

  int WrapV8Array(JNIEnv *jniEnv, v8::Handle<v8::Array> val, jobject *jVal);
  int WrapV8Array(JNIEnv *jniEnv, v8::Handle<v8::Object> val, int componentType, ArrayType *arr, jobject *jVal);
  int ToJavaArray(JNIEnv *jniEnv, v8::Handle<v8::Value> val, int componentType, jobject *jVal);
  int ToV8Array(JNIEnv *jniEnv, jobject jVal, int expectedType, v8::Handle<v8::Object> *val);
  
  int UserGetLength(JNIEnv *jniEnv, v8::Handle<v8::Object> val, int *length);
  int UserSetLength(JNIEnv *jniEnv, v8::Handle<v8::Object> val, int length);
  int UserGetElement(JNIEnv *jniEnv, v8::Handle<v8::Object> val, unsigned int type, int idx, jobject *jVal);
  int UserSetElement(JNIEnv *jniEnv, v8::Handle<v8::Object> val, unsigned int type, int idx, jobject jVal);
  
  int GetRefsForComponentType(JNIEnv *jniEnv, unsigned int componentType, ArrayType **ref);

private:
  Env *env;
  Conv *conv;
  v8::Persistent<v8::String> sArrayHiddenKey;
  v8::Persistent<v8::String> sLength;
  ArrayType *typeToArray[TYPE___END];
  TArray<ArrayType*> *interfaces;
};

} // namespace bridge
#endif
