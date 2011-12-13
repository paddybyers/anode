#include "Interface.h"

#include "defines.h"
#include <string.h>

using namespace v8;

int Interface::Create(JNIEnv *jniEnv, Conv *conv, jobject jInterface, classId class_, int attrCount, int opCount, jclass importStub, jclass exportStub, jclass valueStub, Interface **inst) {
  Interface *ob = new Interface();
  if(!ob) return ErrorMem;
  int result = ob->init(jniEnv, conv, jInterface, class_, attrCount, opCount, importStub, exportStub, valueStub);
  if(result == OK)
    *inst = ob;
  return result;
}

void Interface::dispose(JNIEnv *jniEnv) {
  hiddenKey.Dispose();
  if(jImportStub)
    jniEnv->DeleteGlobalRef(jImportStub);
  if(jExportStub)
    jniEnv->DeleteGlobalRef(jExportStub);
  if(jValueStub)
    jniEnv->DeleteGlobalRef(jValueStub);
  delete[] attributes;
  delete[] operations;
}

int Interface::init(JNIEnv *jniEnv, Conv *conv, jobject jInterface, classId class_, int attrCount, int opCount, jclass importStub, jclass exportStub, jclass valueStub) {
  this->conv       = conv;
  this->jInterface = jInterface;
  this->class_     = class_;
  this->attributes = TArray<Attribute>::New(attrCount);
  this->operations = TArray<Operation>::New(opCount);
  if(!attributes || !operations) return ErrorMem;

  char keyStr[] = "node::interface::xxxx";
  sprintf(&keyStr[17], "%x", class_);
  hiddenKey = Persistent<String>::New(String::New(keyStr));

  if(importStub) {
    jImportStub = (jclass)jniEnv->NewGlobalRef(importStub);
    jImportCtor = jniEnv->GetMethodID(importStub, "<init>", "(JLorg/meshpoint/anode/idl/IDLInterface;)V");
  }

  if(exportStub) {
    jExportStub = (jclass)jniEnv->NewGlobalRef(exportStub);
    jclass classClass = jniEnv->FindClass("java/lang/Class");
    jmethodID classGetName = jniEnv->GetMethodID(classClass, "getName", "()Ljava/lang/String;");
    jstring jExportName = (jstring)jniEnv->CallObjectMethod(exportStub, classGetName);
    const char *exportName = jniEnv->GetStringUTFChars(jExportName, 0);
    int exportNameLen = jniEnv->GetStringUTFLength(jExportName);
    char *methodSig = new char[exportNameLen + sizeof("(L;I[Ljava/lang/Object;)Ljava/lang/Object;")];
    if(!methodSig) return ErrorMem;
    methodSig[0] = '('; methodSig[1] = 'L';
    memcpy(&methodSig[2], exportName, exportNameLen);
    
    memcpy(&methodSig[2 + exportNameLen], ";I[Ljava/lang/Object;)Ljava/lang/Object;", sizeof(";I[Ljava/lang/Object;)Ljava/lang/Object;"));
    jExportInvoke  = jniEnv->GetStaticMethodID(exportStub, "__invoke", methodSig);
    
    memcpy(&methodSig[2 + exportNameLen], ";I)Ljava/lang/Object;", sizeof(";I)Ljava/lang/Object;"));
    jExportGet     = jniEnv->GetStaticMethodID(exportStub, "__get", "methodSig");
    
    memcpy(&methodSig[2 + exportNameLen], ";ILjava/lang/Object;)V", sizeof(";ILjava/lang/Object;)V"));
    jExportSet     = jniEnv->GetStaticMethodID(exportStub, "__set", "");

    jExportGetArgs = jniEnv->GetStaticMethodID(exportStub, "__getArgs", "()[Ljava/lang/Object;");
    
    delete[] methodSig;
  }
  
  if(valueStub) {
    jValueStub    = (jclass)jniEnv->NewGlobalRef(valueStub);
    jValueCtor    = jniEnv->GetMethodID(valueStub, "<init>",    "()V");
    jValueGetArgs = jniEnv->GetMethodID(valueStub, "__getArgs", "()[Ljava/lang/Object;");
    jValueImport  = jniEnv->GetMethodID(valueStub, "__import",  "([Ljava/lang/Object;)V");
    jValueExport  = jniEnv->GetMethodID(valueStub, "__export",  "()[Ljava/lang/Object;");
  }
  return OK;
}

int Interface::initAttribute(JNIEnv *jniEnv, jint idx, jint type, jstring jName) {
  return attributes->addr(idx)->init(jniEnv, conv, type, jName);
}

int Interface::initOperation(JNIEnv *jniEnv, jint idx, jint type, jstring jName, jint argCount, jint *argTypes) {
  return operations->addr(idx)->init(jniEnv, conv, this, type, jName, argCount, argTypes);
}

int Interface::CreateImport(JNIEnv *jniEnv, jlong handle, jobject *jVal) {
  if(!jImportStub) return ErrorNotfound;
  jobject ob = jniEnv->NewObject(jImportStub, jImportCtor, handle, jInterface);
  if(ob) {
    *jVal = ob;
    return OK;
  }
  return ErrorVM;
}

int Interface::CreateValue(JNIEnv *jniEnv, Handle<Object> val, jlong handle, jobject *jVal) {
  jobjectArray args = (jobjectArray)jniEnv->CallStaticObjectMethod(jValueStub, jValueGetArgs);
  jobject ob = jniEnv->NewObject(jValueStub, jValueCtor);
  if(!args || !ob) return ErrorVM;
  jniEnv->MonitorEnter(args);
  int result = OK;
  for(int i = 0; i < attributes->getLength(); i++) {
    jobject jMember;
    Local<Value> member = val->Get(attributes->addr(i)->name);
    result = conv->ToJavaObject(jniEnv, member, attributes->addr(i)->type, &jMember);
    if(result != OK) break;
    jniEnv->SetObjectArrayElement(args, i, jMember);
  }
  if(result == OK) {
    jniEnv->CallVoidMethod(ob, jValueImport, args);
    *jVal = ob;
  }
  jniEnv->MonitorExit(args);  
  return OK;
}

int Interface::ExportValue(JNIEnv *jniEnv, jobject jVal, Handle<Object> val) {
  jobjectArray args = (jobjectArray)jniEnv->CallStaticObjectMethod(jValueStub, jValueGetArgs);
  if(!args) return ErrorVM;
  jniEnv->MonitorEnter(args);
  jniEnv->CallObjectMethod(jVal, jValueExport);
  int result = OK;
  for(int i = 0; i < attributes->getLength(); i++) {
    jobject jMember = jniEnv->GetObjectArrayElement(args, i);
    Local<Value> member;
    result = conv->ToV8Value(jniEnv, jMember, attributes->addr(i)->type, &member);
    if(result != OK) break;
    val->Set(attributes->addr(i)->name, member);
  }
  jniEnv->MonitorExit(args);
  return result;
}

Attribute::~Attribute() {
  if(!name.IsEmpty()) name.Dispose();
}

int Attribute::init(JNIEnv *jniEnv, Conv *conv, jint type, jstring jName) {
  this->type = type;
  Local<String> lName;
  int result = conv->ToV8String(jniEnv, jName, &lName);
  if(result == OK)
    name = Persistent<String>::New(lName);
  return result;
}

Operation::~Operation() {
  delete[] argTypes;
  if(!fInvoke.IsEmpty()) fInvoke.Dispose();
  if(!fGet.IsEmpty()) fGet.Dispose();
  if(!fSet.IsEmpty()) fSet.Dispose();
}

int Operation::init(JNIEnv *jniEnv, Conv *conv, Interface *interface, jint type, jstring jName, jint argCount, jint *argTypes) {
  int result = Attribute::init(jniEnv, conv, type, jName);
  argTypes = new jint[argCount];
  if(result == OK)
    result = argTypes ? OK : ErrorMem;
  return result;
}
