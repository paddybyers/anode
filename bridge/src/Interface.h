#ifndef BRIDGE_INTERFACE_H
#define BRIDGE_INTERFACE_H

#include "defines.h"
#include "Conv.h"
#include "Utils-inl.h"

namespace bridge {

class ArrayType;
class Attribute;
class Operation;

class Interface {
public:
  static int Create(JNIEnv *jniEnv, Env *env, jobject jInterface, classId class_, int attrCount, int opCount, jclass declaredClass, Interface **inst);
  void dispose(JNIEnv *jniEnv);
  ~Interface() {};
  
  int InitUserStub(JNIEnv *jniEnv, jclass userStub);
  int InitPlatformStub(JNIEnv *jniEnv, jclass platformStub);
  int InitDictStub(JNIEnv *jniEnv, jclass dictStub);
  int InitAttribute(JNIEnv *jniEnv, jint idx, jint type, jstring jName);
  int InitOperation(JNIEnv *jniEnv, jint idx, jint type, jstring jName, jint argCount, jint *argTypes);
  
  int DictCreate(JNIEnv *jniEnv, v8::Handle<v8::Object> val, jobject *jVal);
  int DictExport(JNIEnv *jniEnv, jobject jVal, v8::Handle<v8::Object> val);
  
  int UserCreate(JNIEnv *jniEnv, jlong handle, jobject *jVal);
  int UserInvoke(JNIEnv *jniEnv, v8::Handle<v8::Object> target, int opIdx, jobjectArray jArgs, jobject *jResult);
  int UserSet(JNIEnv *jniEnv, v8::Handle<v8::Object> target, int attrIdx, jobject jVal);
  int UserGet(JNIEnv *jniEnv, v8::Handle<v8::Object> target, int attrIdx, jobject *jVal);

  int PlatformCreate(JNIEnv *jniEnv, jobject jVal, v8::Handle<v8::Object> *val);

  inline v8::Persistent<v8::String> getHiddenKey() {return hiddenKey;}
  static inline int classId2Idx(classId class_) {return class_ >> 1;}
  static inline classId idx2ClassId(int idx, bool isDict) {return (idx << 1) + (int)isDict;}
  inline jclass getDeclaredClass() {return declaredClass;}
  
private:
  Interface() : attributes(0), operations(0) {};
  int Init(JNIEnv *jniEnv, Env *env, jobject jInterface, classId class_, int attrCount, int opCount, jclass declaredClass);

  static v8::Handle<v8::Value> PlatformAttrGet(v8::Local<v8::String> property, const v8::AccessorInfo& info);
  static void PlatformAttrSet(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
  static v8::Handle<v8::Value> PlatformOpInvoke(const v8::Arguments& args);
  static v8::Handle<v8::Value> PlatformCtor(const v8::Arguments& args);

  Env *env;
  Conv *conv;
  v8::Persistent<v8::String> hiddenKey;
  v8::Persistent<v8::String> sClassName;
  v8::Persistent<v8::FunctionTemplate> functionTemplate;
  v8::Persistent<v8::Function> function;
  v8::Persistent<v8::ObjectTemplate> instanceTemplate;

  classId class_;
  TArray<Attribute> *attributes;
  TArray<Operation> *operations;
  
  /* JNI */
  jclass    declaredClass;
  jobject   jInterface;

  jclass    jUserStub;
  jmethodID jUserCtor;

  jclass    jPlatformStub;
  jmethodID jPlatformGetArgs;
  jmethodID jPlatformInvoke;
  jmethodID jPlatformGet;
  jmethodID jPlatformSet;
  
  jclass    jDictStub;
  jmethodID jDictCtor;
  jmethodID jDictGetArgs;
  jmethodID jDictImport;
  jmethodID jDictExport;
  
  friend class Attribute;
  friend class Operation;
};

class Attribute {
public:
  int type;
  v8::Persistent<v8::String> name;
  Attribute() {};
  ~Attribute();
  int Init(JNIEnv *jniEnv, Conv *conv, jint type, jstring jName);
};

class Operation : public Attribute {
public:
  int argCount;
  jint *argTypes;
  v8::Handle<v8::Value> *vArgs;
  Operation() : Attribute(), argTypes(0) {};
  ~Operation();
  int Init(JNIEnv *jniEnv, Conv *conv, Interface *interface, jint type, jstring jName, jint argCount, jint *argTypes);
};

} // namespace bridge
#endif
