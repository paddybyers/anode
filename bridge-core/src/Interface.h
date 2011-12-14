#ifndef BRIDGE_INTERFACE_H
#define BRIDGE_INTERFACE_H

#include "defines.h"
#include "Conv.h"
#include "Utils.h"

class Attribute;
class Operation;

using namespace v8;

class Interface {
public:
  static int Create(JNIEnv *jniEnv, Conv *conv, jobject jInterface, classId class_, int attrCount, int opCount, jclass importStub, jclass exportStub, jclass valueStub, Interface **inst);
  void dispose(JNIEnv *jniEnv);
  ~Interface() {};
  
  int initAttribute(JNIEnv *jniEnv, jint idx, jint type, jstring jName);
  int initOperation(JNIEnv *jniEnv, jint idx, jint type, jstring jName, jint argCount, jint *argTypes);
  inline Persistent<String> getHiddenKey() {return hiddenKey;}
  static inline int classId2Idx(classId class_) {return class_ >> 1;}
  
  int DictCreate(JNIEnv *jniEnv, Handle<Object> val, jlong handle, jobject *jVal);
  int DictExport(JNIEnv *jniEnv, jobject jVal, Handle<Object> val);
  
  int UserCreate(JNIEnv *jniEnv, jlong handle, jobject *jVal);
  int UserInvoke(JNIEnv *jniEnv, Handle<Object> target, int opIdx, jobjectArray jArgs, jobject *jResult);
  int UserSet(JNIEnv *jniEnv, Handle<Object> target, int attrIdx, jobject jVal);
  int UserGet(JNIEnv *jniEnv, Handle<Object> target, int attrIdx, jobject *jVal);

  classId operator=(classId) { return this->class_;}
  jobject operator=(jobject) { return this->jInterface;}

private:
  Interface() : attributes(0), operations(0) {};
  int init(JNIEnv *jniEnv, Conv *conv, jobject jInterface, classId class_, int attrCount, int opCount, jclass importStub, jclass exportStub, jclass valueStub);

  Conv *conv;
  Persistent<String> hiddenKey;
  classId class_;
  TArray<Attribute> *attributes;
  TArray<Operation> *operations;
  
  /* JNI */
  jobject   jInterface;

  jclass    jImportStub;
  jmethodID jImportCtor;

  jclass    jExportStub;
  jmethodID jExportGetArgs;
  jmethodID jExportInvoke;
  jmethodID jExportGet;
  jmethodID jExportSet;
  
  jclass    jValueStub;
  jmethodID jValueCtor;
  jmethodID jValueGetArgs;
  jmethodID jValueImport;
  jmethodID jValueExport;
  
  friend class Attribute;
  friend class Operation;
};

class Attribute {
public:
  int type;
  Persistent<String> name;
  Attribute() {};
  virtual ~Attribute();
  int init(JNIEnv *jniEnv, Conv *conv, jint type, jstring jName);
};

class Operation : public Attribute {
public:
  int argCount;
  jint *argTypes;
  Handle<Value> *vArgs;
  Persistent<Function> fInvoke;
  Persistent<Function> fGet;
  Persistent<Function> fSet;
  Operation() : Attribute(), argTypes(0) {};
  virtual ~Operation();
  int init(JNIEnv *jniEnv, Conv *conv, Interface *interface, jint type, jstring jName, jint argCount, jint *argTypes);
};

#endif
