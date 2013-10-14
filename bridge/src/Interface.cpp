/*
 * Copyright 2011-2012 Paddy Byers
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */

#include "Interface.h"

#include "defines.h"
#include <string.h>
#include "ArrayConv.h"
#include "Env.h"
#include "VM.h"

using namespace v8;
using namespace bridge;

int Interface::Create(JNIEnv *jniEnv, Env *env, Interface *parent, jobject jInterface, classId class_, int attrCount, int opCount, jclass declaredClass, Interface **inst) {
  Interface *ob = new Interface();
  if(!ob) return ErrorMem;
  int result = ob->Init(jniEnv, env, parent, jInterface, class_, attrCount, opCount, declaredClass);
  if(result == OK)
    *inst = ob;
  return result;
}

void Interface::dispose(JNIEnv *jniEnv) {
  //LOGV("Interface::dispose: classId = %d\n", class_);
  env->putInterface(class_, 0);
  hiddenKey.Dispose();
  function.Dispose();
  functionTemplate.Dispose();
  if(jUserStub)
    jniEnv->DeleteGlobalRef(jUserStub);
  if(jPlatformStub)
    jniEnv->DeleteGlobalRef(jPlatformStub);
  if(jDictStub)
    jniEnv->DeleteGlobalRef(jDictStub);
  delete attributes;
  delete operations;
}

int Interface::Init(JNIEnv *jniEnv, Env *env, Interface *parent, jobject jInterface, classId class_, int attrCount, int opCount, jclass declaredClass) {
  this->env           = env;
  this->parent        = parent;
  this->conv          = env->getConv();
  this->declaredClass = (jclass)jniEnv->NewGlobalRef(declaredClass);
  this->jInterface    = jInterface;
  this->class_        = class_;
  hiddenKey           = Persistent<String>::New(Conv::getTypeKey(getInterfaceType(class_)));
  sClassName          = Persistent<String>::New(conv->getV8ClassName(jniEnv, declaredClass));  

  jPlatformStub = jUserStub = jDictStub = 0;

  //LOGV("Interface::Init; this = %p; classId = %d; hiddenKey = %p\n", this, class_, hiddenKey);
  this->attributes    = TArray<Attribute>::New(attrCount);
  this->operations    = TArray<Operation>::New(opCount);
  if(!attributes || !operations) return ErrorMem;

  env->putInterface(class_, this);
  return OK;
}

int Interface::InitUserStub(JNIEnv *jniEnv, jclass userStub) {
  jUserStub = (jclass)jniEnv->NewGlobalRef(userStub);
  jUserCtor = jniEnv->GetMethodID(userStub, "<init>", "(J)V");
  return OK;
}

int Interface::InitPlatformStub(JNIEnv *jniEnv, jclass platformStub) {
  jPlatformStub = (jclass)jniEnv->NewGlobalRef(platformStub);
  jstring jPlatformName = conv->getJavaClassName(jniEnv, declaredClass, true);
  const char *platformName = jniEnv->GetStringUTFChars(jPlatformName, 0);
  int nameLen = jniEnv->GetStringUTFLength(jPlatformName);
  char *methodSig = new char[nameLen + sizeof("(L;I[Ljava/lang/Object;)Ljava/lang/Object;")];
  if(!methodSig) return ErrorMem;
  
  if(operations->getLength() > 0) {
    sprintf(methodSig, "(L%s;I[Ljava/lang/Object;)Ljava/lang/Object;", platformName);
    jPlatformInvoke  = jniEnv->GetStaticMethodID(platformStub, "__invoke", methodSig);
    jPlatformGetArgs = jniEnv->GetStaticMethodID(platformStub, "__getArgs", "()[Ljava/lang/Object;");
  }

  if(attributes->getLength() > 0) {
    sprintf(methodSig, "(L%s;I)Ljava/lang/Object;", platformName);
    jPlatformGet     = jniEnv->GetStaticMethodID(platformStub, "__get", methodSig);

    sprintf(methodSig, "(L%s;ILjava/lang/Object;)V", platformName);
    jPlatformSet     = jniEnv->GetStaticMethodID(platformStub, "__set", methodSig);
  }
  
  delete[] methodSig;
  jniEnv->ReleaseStringUTFChars(jPlatformName, platformName);
  
  /* set up function template etc */
  functionTemplate = Persistent<FunctionTemplate>::New(FunctionTemplate::New(PlatformCtor));
  functionTemplate->SetClassName(sClassName);
  
  if(parent != 0) {
    functionTemplate->Inherit(parent->functionTemplate);
  }
  instanceTemplate = Persistent<ObjectTemplate>::New(functionTemplate->InstanceTemplate());
  instanceTemplate->SetInternalFieldCount(1);
  return OK;
}

int Interface::InitDictStub(JNIEnv *jniEnv, jclass dictStub) {
  jDictStub    = (jclass)jniEnv->NewGlobalRef(dictStub);
  jDictCtor    = jniEnv->GetMethodID(declaredClass, "<init>", "()V");
  jDictGetArgs = jniEnv->GetStaticMethodID(dictStub, "__getArgs", "()[Ljava/lang/Object;");

  jstring jDictName = conv->getJavaClassName(jniEnv, declaredClass, true);
  const char *dictName = jniEnv->GetStringUTFChars(jDictName, 0);
  int nameLen = jniEnv->GetStringUTFLength(jDictName);
  char *methodSig = new char[nameLen + sizeof("(L;[Ljava/lang/Object;)V")];
  if(!methodSig) return ErrorMem;
  
  sprintf(methodSig, "(L%s;[Ljava/lang/Object;)V", dictName);
  jDictImport  = jniEnv->GetStaticMethodID(dictStub, "__import", methodSig);
  
  sprintf(methodSig, "(L%s;)[Ljava/lang/Object;", dictName);
  jDictExport  = jniEnv->GetStaticMethodID(dictStub, "__export", methodSig);
  
  delete[] methodSig;
  jniEnv->ReleaseStringUTFChars(jDictName, dictName);
  
  return OK;
}

int Interface::InitAttribute(JNIEnv *jniEnv, jint idx, jint type, jstring jName) {
  Attribute *attr = attributes->addr(idx);
  int result = attr->Init(jniEnv, conv, type, jName);
  if(result == OK && jPlatformStub) {
    char buf[32];
    attr->name->WriteUtf8(buf);
    functionTemplate->InstanceTemplate()->SetAccessor(attr->name, PlatformAttrGet, PlatformAttrSet, Number::New((class_ << 16) + idx));
  }
  return result;
}

int Interface::InitOperation(JNIEnv *jniEnv, jint idx, jint type, jstring jName, jint argCount, jint *argTypes) {
  Operation *op = operations->addr(idx);
  int result = op->Init(jniEnv, conv, this, type, jName, argCount, argTypes);
  if(result == OK && jPlatformStub) {
    Local<FunctionTemplate> opTemp = FunctionTemplate::New(PlatformOpInvoke, Number::New((class_ << 16) + idx));
    functionTemplate->PrototypeTemplate()->Set(op->name, opTemp);
  }
  return result;
}

int Interface::UserCreate(JNIEnv *jniEnv, jlong handle, jobject *jVal) {
  if(!jUserStub) return ErrorInternal;
  jobject ob = jniEnv->NewObject(jUserStub, jUserCtor, handle);
  if(ob) {
    *jVal = ob;
    return OK;
  }
  return ErrorVM;
}

int Interface::DictCreate(JNIEnv *jniEnv, Handle<Object> val, jobject *jVal) {
  if(!jDictStub) return ErrorInternal;
  jobject ob = jniEnv->NewObject(declaredClass, jDictCtor);
  if(!ob) return ErrorVM;
  int result = DictImport(jniEnv, val, ob);
  if(result == OK) {
    *jVal = ob;
  }
  return result;
}

int Interface::DictImport(JNIEnv *jniEnv, Handle<Object> val, jobject jVal) {
  jobjectArray args = (jobjectArray)jniEnv->CallStaticObjectMethod(jDictStub, jDictGetArgs);
  jniEnv->MonitorEnter(args);
  int result = OK;
  for(int i = 0; i < attributes->getLength(); i++) {
    jobject jMember;
    Handle<String> attrName = attributes->addr(i)->name;
    Local<Value> member = val->Has(attrName) ? val->Get(attrName) : Local<Value>();
    result = conv->ToJavaObject(jniEnv, member, attributes->addr(i)->type, &jMember);
    if(result != OK) break;
    jniEnv->SetObjectArrayElement(args, i, jMember);
    jniEnv->DeleteLocalRef(jMember);
  }
  if(result == OK) {
    jniEnv->CallStaticVoidMethod(jDictStub, jDictImport, jVal, args);
  }
  jniEnv->MonitorExit(args);
  jniEnv->DeleteLocalRef(args);
  if(result == OK && parent != 0) {
    result = parent->DictImport(jniEnv, val, jVal);
  }
  return result;
}

int Interface::UserInvoke(JNIEnv *jniEnv, Handle<Object> target, int opIdx, jobjectArray jArgs, jobject *jResult) {
  HandleScope scope;
  TryCatch tryCatch;
  Operation *op = operations->addr(opIdx);
  int result = OK;
  jobject ob = 0;
  // This fails because passing references from one frame to the parent frame is broken:
  // https://code.google.com/p/android/issues/detail?id=15119
  //jniEnv->PushLocalFrame(512);
  for(int i = 0; result == OK && i < op->argCount; i++) {
      jobject jMember = jniEnv->GetObjectArrayElement(jArgs, i);
      result = conv->ToV8Value(jniEnv, jMember, op->argTypes[i], &op->vArgs[i]);
      jniEnv->DeleteLocalRef(jMember);
  }
  if(result == OK) {
    Handle<Value> vRes;
    if(target->IsFunction() && parent == 0 && operations->getLength() == 1) {
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
      result = conv->ToJavaObject(jniEnv, vRes, op->type, &ob);
    }
  }
  //*jResult = jniEnv->PopLocalFrame(ob);
  *jResult = ob;
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
  if(!jDictStub) return ErrorInternal;
  jobjectArray args = (jobjectArray)jniEnv->CallStaticObjectMethod(jDictStub, jDictGetArgs);
  if(!args) return ErrorVM;
  jniEnv->MonitorEnter(args);
  jobject ensureNotLeakedRef = jniEnv->CallStaticObjectMethod(jDictStub, jDictExport, jVal);
  int result = OK;
  for(int i = 0; i < attributes->getLength(); i++) {
    jobject jMember = jniEnv->GetObjectArrayElement(args, i);
    Local<Value> member;
    result = conv->ToV8Value(jniEnv, jMember, attributes->addr(i)->type, &member);
    jniEnv->DeleteLocalRef(jMember);
    if(result != OK) break;
    val->Set(attributes->addr(i)->name, member);
  }
  jniEnv->MonitorExit(args);
  jniEnv->DeleteLocalRef(ensureNotLeakedRef);
  jniEnv->DeleteLocalRef(args);
  if(result == OK && parent != 0) {
    result = parent->DictExport(jniEnv, jVal, val);
  }
  return result;
}

int Interface::PlatformCreate(JNIEnv *jniEnv, jobject jVal, v8::Handle<v8::Object> *val) {
  //LOGV("Interface::PlatformCreate: ent, this=%p\n", this);
  if(!jPlatformStub) return ErrorInternal;
  if(function.IsEmpty()) function = Persistent<Function>::New(functionTemplate->GetFunction());
  Local<Object> local = function->NewInstance();
  int result = local.IsEmpty() ? ErrorVM : OK;
  if(result == OK) {
    *val = local;
  }
  //LOGV("Interface::PlatformCreate: ret, result=%d\n", result);
  return result;
}

Handle<Value> Interface::PlatformAttrGet(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  jobject ob = (jobject)info.This()->GetPointerFromInternalField(0);
  int attrData = info.Data()->Int32Value();
  int attrIdx = attrData & 0xffff;
  int clsid = attrData >> 16;
  Env *env = Env::getEnv_nocheck();
  Interface *interface = env->getInterface(clsid);
  if(interface) {
    Attribute *attr = interface->attributes->addr(attrIdx);
    JNIEnv *jniEnv = interface->env->getVM()->getJNIEnv();
    jniEnv->PushLocalFrame(256);
    jobject jVal = jniEnv->CallStaticObjectMethod(interface->jPlatformStub, interface->jPlatformGet, ob, attrIdx);
    if(env->getConv()->CheckForException(jniEnv)) {
      jniEnv->PopLocalFrame(0);
      return Undefined();
    }
    Local<Value> val;
    int result = interface->conv->ToV8Value(jniEnv, jVal, attr->type, &val);
    jniEnv->PopLocalFrame(0);
    if(result == OK) {
      return scope.Close(val);
    }
    interface->conv->ThrowV8ExceptionForErrno(result);
  }
  return Undefined();
}

void Interface::PlatformAttrSet(Local<String> property, Local<Value> value, const AccessorInfo& info) {
  HandleScope scope;
  jobject ob = (jobject)info.This()->GetPointerFromInternalField(0);
  int attrData = info.Data()->Int32Value();
  int attrIdx = attrData & 0xffff;
  int clsid = attrData >> 16;
  Env *env = Env::getEnv_nocheck();
  Interface *interface = env->getInterface(clsid);
  if(interface) {
    Attribute *attr = interface->attributes->addr(attrIdx);
    JNIEnv *jniEnv = interface->env->getVM()->getJNIEnv();
    jniEnv->PushLocalFrame(256);
    jobject jVal;
    int result = interface->conv->ToJavaObject(jniEnv, value, attr->type, &jVal);
    if(result == OK) {
      jniEnv->CallStaticVoidMethod(interface->jPlatformStub, interface->jPlatformSet, ob, attrIdx, jVal);
    }
    jniEnv->PopLocalFrame(0);
    if(result == OK)
      env->getConv()->CheckForException(jniEnv);
    else
      interface->conv->ThrowV8ExceptionForErrno(result);
  }
}

Handle<Value> Interface::PlatformOpInvoke(const Arguments& args) {
  HandleScope scope;
  jobject ob = (jobject)args.This()->GetPointerFromInternalField(0);
  int opData = args.Data()->Int32Value();
  int opIdx = opData & 0xffff;
  int clsid = opData >> 16;
  Env *env = Env::getEnv_nocheck();
  Interface *interface = env->getInterface(clsid);
  if(interface) {
    Operation *op = interface->operations->addr(opIdx);
    JNIEnv *jniEnv = interface->env->getVM()->getJNIEnv();
    jniEnv->PushLocalFrame(256);
    int result = OK;
    jobjectArray jArgs = 0;
    Local<Value> val;
    int expectedArgs = op->argCount;
    if(expectedArgs > 0) {
      jArgs = (jobjectArray)jniEnv->CallStaticObjectMethod(interface->jPlatformStub, interface->jPlatformGetArgs);
      jniEnv->MonitorEnter(jArgs);
      int suppliedArgs = args.Length();
      int argsToProcess = (suppliedArgs < expectedArgs) ? suppliedArgs : expectedArgs;
      jobject jItem;
      for(int i = 0; i < argsToProcess; i++) {
        result = interface->conv->ToJavaObject(jniEnv, args[i], op->argTypes[i], &jItem);
        if(result != OK) break;
        jniEnv->SetObjectArrayElement(jArgs, i, jItem);
        jniEnv->DeleteLocalRef(jItem);
      }
      if(result == OK) {
        for(int i = argsToProcess; i < expectedArgs; i++) {
          result = interface->conv->ToJavaObject(jniEnv, Undefined(), op->argTypes[i], &jItem);
          if(result != OK) break;
          jniEnv->SetObjectArrayElement(jArgs, i, jItem);
          jniEnv->DeleteLocalRef(jItem);
        }
      }
    }
    if(result == OK) {
      jobject jVal = jniEnv->CallStaticObjectMethod(interface->jPlatformStub, interface->jPlatformInvoke, ob, opIdx, jArgs);
      if(env->getConv()->CheckForException(jniEnv)) {
        return Undefined();
      }
      if(result == OK) {
        result = interface->conv->ToV8Value(jniEnv, jVal, op->type, &val);
      }
    }
    if(expectedArgs > 0) {
      jniEnv->MonitorExit(jArgs);
    }
    jniEnv->PopLocalFrame(0);
    if(result == OK && !val.IsEmpty()) {
      return scope.Close(val);
    }
    interface->conv->ThrowV8ExceptionForErrno(result);
    LOGV("Interface::PlatformOpInvoke(): ret (error), result = %d\n", result);
  }
  return Undefined(); 
}

Handle<Value> Interface::PlatformCtor(const Arguments& args) {
  return args.This();
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
  this->argCount = argCount;
  this->argTypes = new jint[argCount];
  vArgs = new Handle<Value>[argCount];
  if(result == OK)
    result = (this->argTypes && vArgs) ? OK : ErrorMem;
  if(result == OK) {
    memcpy(this->argTypes, argTypes, argCount * sizeof(jint));
  }
  return result;
}
