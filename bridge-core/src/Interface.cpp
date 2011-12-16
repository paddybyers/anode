#include "Interface.h"

#include "defines.h"
#include <string.h>
#include "ArrayConv.h"
#include "Env.h"
#include "VM.h"
#include "Utils.cpp"

using namespace v8;
using namespace bridge;

int Interface::Create(JNIEnv *jniEnv, Env *env, jobject jInterface, classId class_, int attrCount, int opCount, jclass declaredClass, Interface **inst) {
  Interface *ob = new Interface();
  if(!ob) return ErrorMem;
  int result = ob->Init(jniEnv, env, jInterface, class_, attrCount, opCount, declaredClass);
  if(result == OK)
    *inst = ob;
  return result;
}

void Interface::dispose(JNIEnv *jniEnv) {
  hiddenKey.Dispose();
  function.Dispose();
  functionTemplate.Dispose();
  if(jUserStub)
    jniEnv->DeleteGlobalRef(jUserStub);
  if(jPlatformStub)
    jniEnv->DeleteGlobalRef(jPlatformStub);
  if(jDictStub)
    jniEnv->DeleteGlobalRef(jDictStub);
  delete[] attributes;
  delete[] operations;
}

int Interface::Init(JNIEnv *jniEnv, Env *env, jobject jInterface, classId class_, int attrCount, int opCount, jclass declaredClass) {
  this->env           = env;
  this->conv          = env->getConv();
  this->declaredClass = declaredClass;
  this->jInterface    = jInterface;
  this->class_        = class_;
  this->attributes    = TArray<Attribute>::New(attrCount);
  this->operations    = TArray<Operation>::New(opCount);
  if(!attributes || !operations) return ErrorMem;

  hiddenKey = Persistent<String>(Conv::getTypeKey(getInterfaceType(class_)));
  sClassName = Persistent<String>::New(conv->getV8ClassName(jniEnv, declaredClass));
  
  return OK;
}

int Interface::InitUserStub(JNIEnv *jniEnv, jclass userStub) {
  jUserStub = (jclass)jniEnv->NewGlobalRef(userStub);
  jUserCtor = jniEnv->GetMethodID(userStub, "<init>", "(JLorg/meshpoint/anode/idl/IDLInterface;)V");
  return OK;
}

int Interface::InitPlatformStub(JNIEnv *jniEnv, jclass platformStub) {
  jPlatformStub = (jclass)jniEnv->NewGlobalRef(platformStub);
  jstring jPlatformName = conv->getJavaClassName(jniEnv, platformStub);
  const char *platformName = jniEnv->GetStringUTFChars(jPlatformName, 0);
  char *methodSig = new char[jniEnv->GetStringUTFLength(jPlatformName) + sizeof("(L;I[Ljava/lang/Object;)Ljava/lang/Object;")];
  if(!methodSig) return ErrorMem;
  
  sprintf(methodSig, "(L%s;I[Ljava/lang/Object;)Ljava/lang/Object;", platformName);
  jPlatformInvoke  = jniEnv->GetStaticMethodID(platformStub, "__invoke", methodSig);
  
  sprintf(methodSig, "(L%s;I)Ljava/lang/Object;", platformName);
  jPlatformGet     = jniEnv->GetStaticMethodID(platformStub, "__get", "methodSig");
  
  sprintf(methodSig, "(L%s;ILjava/lang/Object;)V", platformName);
  jPlatformSet     = jniEnv->GetStaticMethodID(platformStub, "__set", "");
  
  jPlatformGetArgs = jniEnv->GetStaticMethodID(platformStub, "__getArgs", "()[Ljava/lang/Object;");
  
  delete[] methodSig;
  jniEnv->ReleaseStringUTFChars(jPlatformName, platformName);
  
  /* set up function template etc */
  functionTemplate = Persistent<FunctionTemplate>::New(FunctionTemplate::New());
  functionTemplate->SetClassName(sClassName);
  instanceTemplate = Persistent<ObjectTemplate>::New(functionTemplate->InstanceTemplate());
  instanceTemplate->SetInternalFieldCount(2);
  return OK;
}

int Interface::InitDictStub(JNIEnv *jniEnv, jclass dictStub) {
  jDictStub    = (jclass)jniEnv->NewGlobalRef(dictStub);
  jDictCtor    = jniEnv->GetMethodID(dictStub, "<init>",    "()V");
  jDictGetArgs = jniEnv->GetMethodID(dictStub, "__getArgs", "()[Ljava/lang/Object;");
  jDictImport  = jniEnv->GetMethodID(dictStub, "__import",  "([Ljava/lang/Object;)V");
  jDictExport  = jniEnv->GetMethodID(dictStub, "__export",  "()[Ljava/lang/Object;");
  return OK;
}

int Interface::InitAttribute(JNIEnv *jniEnv, jint idx, jint type, jstring jName) {
  Attribute *attr = attributes->addr(idx);
  int result = attr->Init(jniEnv, conv, type, jName);
  if(result == OK && jPlatformStub) {
    instanceTemplate->SetAccessor(attr->name, PlatformAttrGet, PlatformAttrSet, Number::New(idx));
  }
  return result;
}

int Interface::InitOperation(JNIEnv *jniEnv, jint idx, jint type, jstring jName, jint argCount, jint *argTypes) {
  Operation *op = operations->addr(idx);
  int result = op->Init(jniEnv, conv, this, type, jName, argCount, argTypes);
  if(result == OK && jPlatformStub) {
    Local<FunctionTemplate> opTemp = FunctionTemplate::New(PlatformOpInvoke, Number::New(idx));
    functionTemplate->PrototypeTemplate()->Set(op->name, opTemp);
  }
  return result;
}

int Interface::UserCreate(JNIEnv *jniEnv, jlong handle, jobject *jVal) {
  if(!jUserStub) return ErrorNotfound;
  jobject ob = jniEnv->NewObject(jUserStub, jUserCtor, handle, jInterface);
  if(ob) {
    *jVal = ob;
    return OK;
  }
  return ErrorVM;
}

int Interface::DictCreate(JNIEnv *jniEnv, Handle<Object> val, jobject *jVal) {
  jobjectArray args = (jobjectArray)jniEnv->CallStaticObjectMethod(jDictStub, jDictGetArgs);
  jobject ob = jniEnv->NewObject(jDictStub, jDictCtor);
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
    jniEnv->CallVoidMethod(ob, jDictImport, args);
    *jVal = ob;
  }
  jniEnv->MonitorExit(args);  
  return result;
}

int Interface::UserInvoke(JNIEnv *jniEnv, Handle<Object> target, int opIdx, jobjectArray jArgs, jobject *jResult) {
  HandleScope scope;
  TryCatch tryCatch;
  Operation *op = operations->addr(opIdx);
  int result = OK;
  for(int i = 0; result == OK && i < op->argCount; i++) {
      result = conv->ToV8Value(jniEnv, jniEnv->GetObjectArrayElement(jArgs, i), op->argTypes[i], &op->vArgs[i]);
  }
  if(result == OK) {
    Handle<Value> vRes;
    if(target->IsFunction() && op->argCount == 1) {
      /* invoke as function if target is a function, and interface delcares only one operation */
      vRes = (Handle<Function>::Cast(target))->Call(target, op->argCount, op->vArgs);
    } else {
      /* locate the method and invoke that */
      Handle<Value> vMethod = target->Get(op->name);
      if(!vMethod.IsEmpty() && vMethod->IsFunction()) {
        vRes = Handle<Function>::Cast(vMethod)->Call(target, op->argCount, op->vArgs);
      }
    }
    if(!vRes.IsEmpty() && op->type != TYPE_UNDEFINED) {
      jobject ob;
      result = conv->ToJavaObject(jniEnv, vRes, op->type, &ob);
      if(result == OK) {
        *jResult = ob;
      }
    }
  }
  if(tryCatch.HasCaught()) {
    result = ErrorJS;
    tryCatch.Reset();
  }
  return result;
}

int Interface::UserSet(JNIEnv *jniEnv, Handle<Object> target, int attrIdx, jobject jVal) {
  HandleScope scope;
  TryCatch tryCatch;
  Attribute *attr = attributes->addr(attrIdx);
  Handle<Value> val;
  int result = conv->ToV8Value(jniEnv, jVal, attr->type, &val);
  if(result == OK) {
    target->Set(attr->name, val);
  }
  if(tryCatch.HasCaught()) {
    result = ErrorJS;
    tryCatch.Reset();
  }
  return result;
}

int Interface::UserGet(JNIEnv *jniEnv, Handle<Object> target, int attrIdx, jobject *jVal) {
  HandleScope scope;
  TryCatch tryCatch;
  Attribute *attr = attributes->addr(attrIdx);
  Handle<Value> val = target->Get(attr->name);
  if(tryCatch.HasCaught()) {
    tryCatch.Reset();
    return ErrorJS;
  }
  jobject ob;
  int result = conv->ToJavaObject(jniEnv, val, attr->type, &ob);
  if(result == OK) {
    *jVal = ob;
  }
  return result;
}

int Interface::DictExport(JNIEnv *jniEnv, jobject jVal, Handle<Object> val) {
  jobjectArray args = (jobjectArray)jniEnv->CallStaticObjectMethod(jDictStub, jDictGetArgs);
  if(!args) return ErrorVM;
  jniEnv->MonitorEnter(args);
  jniEnv->CallObjectMethod(jVal, jDictExport);
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

int Interface::PlatformCreate(JNIEnv *jniEnv, jobject jVal, v8::Handle<v8::Object> *val) {
  Local<Object> local = function->NewInstance();
  int result = local.IsEmpty() ? ErrorVM : OK;
  if(result == OK) {
    local->SetPointerInInternalField(1, this);
    *val = local;
  }
  return result;
}

Handle<Value> Interface::PlatformAttrGet(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  jobject ob = (jobject)info.This()->GetPointerFromInternalField(0);
  Interface *interface = (Interface *)info.This()->GetPointerFromInternalField(1);
  int attrIdx = info.Data()->Int32Value();
  Attribute *attr = interface->attributes->addr(attrIdx);
  JNIEnv *jniEnv = interface->env->getVM()->getJNIEnv();
  jobject jVal = jniEnv->CallStaticObjectMethod(interface->jPlatformStub, interface->jPlatformGet, ob, attrIdx);
  if(jniEnv->ExceptionCheck()) {
    jniEnv->ExceptionClear();
    ThrowException(Exception::Error(String::New("FIXME: Unknown error")));
    return Undefined();
  }
  Local<Value> val;
  int result = interface->conv->ToV8Value(jniEnv, jVal, attr->type, &val);
  if(result == OK) {
    return scope.Close(val);
  }
  interface->conv->ThrowV8ExceptionForErrno(result);
  return Undefined();
}

void Interface::PlatformAttrSet(Local<String> property, Local<Value> value, const AccessorInfo& info) {
  HandleScope scope;
  jobject ob = (jobject)info.This()->GetPointerFromInternalField(0);
  Interface *interface = (Interface *)info.This()->GetPointerFromInternalField(1);
  int attrIdx = info.Data()->Int32Value();
  Attribute *attr = interface->attributes->addr(attrIdx);
  JNIEnv *jniEnv = interface->env->getVM()->getJNIEnv();
  jobject jVal;
  int result = interface->conv->ToJavaObject(jniEnv, value, attr->type, &jVal);
  if(result == OK) {
    jniEnv->CallStaticVoidMethod(interface->jPlatformStub, interface->jPlatformSet, ob, attrIdx, jVal);
    if(jniEnv->ExceptionCheck()) {
      jniEnv->ExceptionClear();
      ThrowException(Exception::Error(String::New("FIXME: Unknown error")));
      return;
    }
  }
  if(result != OK)
    interface->conv->ThrowV8ExceptionForErrno(result);
}

Handle<Value> Interface::PlatformOpInvoke(const Arguments& args) {
  HandleScope scope;
  jobject ob = (jobject)args.This()->GetPointerFromInternalField(0);
  Interface *interface = (Interface *)args.This()->GetPointerFromInternalField(1);
  int opIdx = args.Data()->Int32Value();
  Operation *op = interface->operations->addr(opIdx);
  JNIEnv *jniEnv = interface->env->getVM()->getJNIEnv();
  jobjectArray jArgs = (jobjectArray)jniEnv->CallStaticObjectMethod(interface->jPlatformStub, interface->jPlatformGetArgs);
  jniEnv->MonitorEnter(jArgs);
  int suppliedArgs = args.Length();
  int expectedArgs = op->argCount;
  int argsToProcess = (suppliedArgs < expectedArgs) ? suppliedArgs : expectedArgs;
  int result = OK;
  Local<Value> val;
  jobject jItem;
  for(int i = 0; i < argsToProcess; i++) {
    result = interface->conv->ToJavaObject(jniEnv, args[i], op->argTypes[i], &jItem);
    if(result != OK) break;
    jniEnv->SetObjectArrayElement(jArgs, i, jItem);    
  }
  if(result == OK) {
    for(int i = argsToProcess; i < expectedArgs; i++) {
      result = interface->conv->ToJavaObject(jniEnv, Undefined(), op->argTypes[i], &jItem);
      if(result != OK) break;
      jniEnv->SetObjectArrayElement(jArgs, i, jItem);
    }
    if(result == OK) {
      jobject jVal = jniEnv->CallStaticObjectMethod(interface->jPlatformStub, interface->jPlatformInvoke, ob, opIdx, jArgs);
      if(jniEnv->ExceptionCheck()) {
        jniEnv->ExceptionClear();
        result = ErrorVM;
      }
      if(result == OK) {
        result = interface->conv->ToV8Value(jniEnv, jVal, op->type, &val);
      }
    }
  }
  jniEnv->MonitorExit(jArgs);
  if(result == OK && !val.IsEmpty()) {
    return scope.Close(val);
  }
  interface->conv->ThrowV8ExceptionForErrno(result);
  return Undefined();  
}

Attribute::~Attribute() {
  if(!name.IsEmpty()) name.Dispose();
}

int Attribute::Init(JNIEnv *jniEnv, Conv *conv, jint type, jstring jName) {
  this->type = type;
  Local<String> lName;
  int result = conv->ToV8String(jniEnv, jName, &lName);
  if(result == OK) {
    name = Persistent<String>::New(lName);
  }
  return result;
}

Operation::~Operation() {
  delete[] argTypes;
  delete[] vArgs;
}

int Operation::Init(JNIEnv *jniEnv, Conv *conv, Interface *interface, jint type, jstring jName, jint argCount, jint *argTypes) {
  int result = Attribute::Init(jniEnv, conv, type, jName);
  argTypes = new jint[argCount];
  vArgs = new Handle<Value>[argCount];
  if(result == OK)
    result = (argTypes && vArgs) ? OK : ErrorMem;
  return result;
}
