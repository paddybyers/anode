#ifndef BRIDGE_ARRAYCONV_H
#define BRIDGE_ARRAYCONV_H

#include "defines.h"
#include "Conv.h"
#include "Utils.h"

namespace bridge {

class ArrayType;
class Env;

typedef v8::Handle<v8::Value> (*eltGetter)(ArrayType *arr, JNIEnv *jniEnv, jobject ob, uint32_t index);
typedef void (*eltSetter)(ArrayType *arr, JNIEnv *jniEnv, jobject ob, uint32_t index, v8::Handle<v8::Value>);
  
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
            const char *javaGetterSig,
            const char *javaSetterSig,
            eltGetter getter = 0,
            eltSetter setter = 0
  );
  ~ArrayType() {}
  int UserNew(JNIEnv *jniEnv, jlong handle, jobject *jVal);
  int PlatformNew(JNIEnv *jniEnv, v8::Handle<v8::Object> *val);
  void dispose(JNIEnv *jniEnv);
private:
  Env *env;
  eltGetter getter;
  eltSetter setter;
  static v8::Handle<v8::Value> PlatformLengthGet(v8::Local<v8::String> property, const v8::AccessorInfo& info);
  static void PlatformLengthSet(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
  static v8::Handle<v8::Value> PlatformElementGet(uint32_t index, const v8::AccessorInfo& info);
  static v8::Handle<v8::Value> PlatformElementSet(uint32_t index, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
  static v8::Handle<v8::Value> PlatformCtor(const v8::Arguments& args);

  static v8::Handle<v8::Value> ByteGet(ArrayType *arr, JNIEnv *jniEnv, jobject ob, uint32_t index);
  static v8::Handle<v8::Value> IntegerGet(ArrayType *arr, JNIEnv *jniEnv, jobject ob, uint32_t index);
  static v8::Handle<v8::Value> LongGet(ArrayType *arr, JNIEnv *jniEnv, jobject ob, uint32_t index);
  static v8::Handle<v8::Value> DoubleGet(ArrayType *arr, JNIEnv *jniEnv, jobject ob, uint32_t index);
  static void ByteSet(ArrayType *arr, JNIEnv *jniEnv, jobject ob, uint32_t index, v8::Handle<v8::Value> elt);
  static void IntegerSet(ArrayType *arr, JNIEnv *jniEnv, jobject ob, uint32_t index, v8::Handle<v8::Value> elt);
  static void LongSet(ArrayType *arr, JNIEnv *jniEnv, jobject ob, uint32_t index, v8::Handle<v8::Value> elt);
  static void DoubleSet(ArrayType *arr, JNIEnv *jniEnv, jobject ob, uint32_t index, v8::Handle<v8::Value> elt);

  friend class ArrayConv;
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
  
  friend class ArrayType;
};

} // namespace bridge
#endif
