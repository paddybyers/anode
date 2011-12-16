#include "ArrayConv.h"
#include "Env.h"
#include "VM.h"
#include "Utils.cpp"
#include <string.h>
#include <math.h>

using namespace v8;
using namespace bridge;

ArrayType::ArrayType(Env *env,
                     JNIEnv *jniEnv,
                     unsigned int componentType,
                     const char *ClassName,
                     const char *jsCtor,
                     const char *javaGetter,
                     const char *javaSetter)
{
  this->env           = env;
  this->componentType = componentType;
  char nameBuf[64];
  sprintf(nameBuf, "org/meshpoint/anode/js/JS%s", ClassName);
  js.class_   = (jclass)jniEnv->NewGlobalRef(jniEnv->FindClass(nameBuf));
  sprintf(nameBuf, "org/meshpoint/anode/java/%s", ClassName);
  java.class_ = (jclass)jniEnv->NewGlobalRef(jniEnv->FindClass(nameBuf));
  js.ctor     = jniEnv->GetMethodID(js.class_,   "<init>",     "(J)V");
  getLength   = jniEnv->GetMethodID(java.class_, "getLength",  "()I");
  setLength   = jniEnv->GetMethodID(java.class_, "setLength",  "(I)V");
  getElement  = jniEnv->GetMethodID(java.class_, "getElement", javaGetter);
  setElement  = jniEnv->GetMethodID(java.class_, "setElement", javaSetter);
  sHiddenKey  = Persistent<String>(Conv::getTypeKey(TYPE_ARRAY|componentType));
  sClassName  = Persistent<String>(String::New(ClassName));
}

int ArrayType::UserNew(JNIEnv *jniEnv, jobject *jVal) {
  if(isJavaObject(componentType))
    *jVal = jniEnv->NewObject(js.class_, js.ctor, componentType);
  else
    *jVal = jniEnv->NewObject(js.class_, js.ctor);
  return *jVal ? OK : ErrorVM;
}

int ArrayType::PlatformNew(JNIEnv *jniEnv, Handle<Object> *val) {
  int result = OK;
  if(function.IsEmpty()) {
    functionTemplate = Persistent<FunctionTemplate>::New(FunctionTemplate::New());
    functionTemplate->SetClassName(sClassName);
    Local<ObjectTemplate> instanceTemplate = functionTemplate->InstanceTemplate();
    instanceTemplate->SetInternalFieldCount(2);
    instanceTemplate->SetAccessor(env->getConv()->getArrayConv()->getSLength(), UserLengthGet, UserLengthSet);
    instanceTemplate->SetIndexedPropertyHandler(UserElementGet, UserElementSet);
    function = Persistent<Function>::New(functionTemplate->GetFunction());
  }
  if(result == OK) {
    Local<Object> local = function->NewInstance();
    result = local.IsEmpty() ? ErrorVM : OK;
    if(result == OK) {
      local->SetPointerInInternalField(1, this);
      *val = local;
    }
  }
  return result;
}

Handle<Value> ArrayType::UserLengthGet(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  jobject ob = (jobject)info.This()->GetPointerFromInternalField(0);
  ArrayType *arr = (ArrayType *)info.This()->GetPointerFromInternalField(1);
  JNIEnv *jniEnv = arr->env->getVM()->getJNIEnv();
  jint length = jniEnv->CallIntMethod(ob, arr->getLength);
  if(jniEnv->ExceptionCheck()) {
    jniEnv->ExceptionClear();
    ThrowException(Exception::Error(String::New("FIXME: Unknown error")));
    return Undefined();
  }
  return scope.Close(Integer::New(length));
}

void ArrayType::UserLengthSet(Local<String> property, Local<Value> value, const AccessorInfo& info) {
  HandleScope scope;
  jobject ob = (jobject)info.This()->GetPointerFromInternalField(0);
  ArrayType *arr = (ArrayType *)info.This()->GetPointerFromInternalField(1);
  JNIEnv *jniEnv = arr->env->getVM()->getJNIEnv();
  if(value.IsEmpty()) {
    ThrowException(Exception::ReferenceError(String::New("No value specified")));
    return;
  }
  jint length = (jint)value->IntegerValue();
  jniEnv->CallVoidMethod(ob, arr->setLength, length);
  if(jniEnv->ExceptionCheck()) {
    jniEnv->ExceptionClear();
    ThrowException(Exception::Error(String::New("FIXME: Unknown error")));
  }
}

Handle<Value> ArrayType::UserElementGet(uint32_t index, const AccessorInfo& info) {
  HandleScope scope;
  jobject ob = (jobject)info.This()->GetPointerFromInternalField(0);
  ArrayType *arr = (ArrayType *)info.This()->GetPointerFromInternalField(1);
  JNIEnv *jniEnv = arr->env->getVM()->getJNIEnv();
  jobject jVal = jniEnv->CallObjectMethod(ob, arr->getElement, index);
  if(jniEnv->ExceptionCheck()) {
    jniEnv->ExceptionClear();
    return Undefined();
  }
  Local<Value> val;
  int result = arr->env->getConv()->ToV8Value(jniEnv, jVal, arr->componentType, &val);
  if(result == OK) {
    return scope.Close(val);
  }
  Conv::ThrowV8ExceptionForErrno(result);
  return Undefined();
}

Handle<Value> ArrayType::UserElementSet(uint32_t index, Local<Value> value, const AccessorInfo& info) {
  HandleScope scope;
  jobject ob = (jobject)info.This()->GetPointerFromInternalField(0);
  ArrayType *arr = (ArrayType *)info.This()->GetPointerFromInternalField(1);
  JNIEnv *jniEnv = arr->env->getVM()->getJNIEnv();
  jobject jVal;
  int result = arr->env->getConv()->ToJavaObject(jniEnv, value, arr->componentType, &jVal);
  if(result == OK) {
    jniEnv->CallVoidMethod(ob, arr->setElement, index, jVal);
    if(jniEnv->ExceptionCheck()) {
      jniEnv->ExceptionClear();
      result = ErrorVM;
    }
  }
  if(result != OK)
    Conv::ThrowV8ExceptionForErrno(result);

  return scope.Close(value);
}

void ArrayType::dispose(JNIEnv *jniEnv) {
  jniEnv->DeleteGlobalRef(js.class_);
  jniEnv->DeleteGlobalRef(java.class_);
  sHiddenKey.Dispose();
  sClassName.Dispose();
  function.Dispose();
  functionTemplate.Dispose();
}

ArrayConv::ArrayConv(Env *env, Conv *conv, JNIEnv *jniEnv) {
  this->env = env;
  this->conv = conv;
  
  sArrayHiddenKey = Persistent<String>::New(String::NewSymbol("node::array"));
  sLength         = Persistent<String>::New(String::NewSymbol("length"));

  memset(&typeToArray, 0, sizeof(typeToArray));
  typeToArray[TYPE_BYTE]     = new ArrayType(env, jniEnv, TYPE_BYTE,     "ByteArray",    "(J)V", "(I)B", "(IB)V");
  typeToArray[TYPE_INT]      = new ArrayType(env, jniEnv, TYPE_INT,      "IntegerArray", "(J)V", "(I)I", "(II)V");
  typeToArray[TYPE_LONG]     = new ArrayType(env, jniEnv, TYPE_LONG,     "LongArray",    "(J)V", "(I)J", "(IJ)V");
  typeToArray[TYPE_DOUBLE]   = new ArrayType(env, jniEnv, TYPE_DOUBLE,   "DoubleArray",  "(J)V", "(I)D", "(ID)V");
  typeToArray[TYPE_STRING]   = new ArrayType(env, jniEnv, TYPE_STRING,   "ObjectArray",  "(J)V", "(I)Ljava/lang/Object;", "(ILjava/lang/Object;)V");
  typeToArray[TYPE_MAP]      = new ArrayType(env, jniEnv, TYPE_MAP,      "ObjectArray",  "(J)V", "(I)Ljava/lang/Object;", "(ILjava/lang/Object;)V");
  typeToArray[TYPE_FUNCTION] = new ArrayType(env, jniEnv, TYPE_FUNCTION, "ObjectArray",  "(J)V", "(I)Ljava/lang/Object;", "(ILjava/lang/Object;)V");
  typeToArray[TYPE_DATE]     = new ArrayType(env, jniEnv, TYPE_DATE,     "ObjectArray",  "(J)V", "(I)Ljava/lang/Object;", "(ILjava/lang/Object;)V");
  typeToArray[TYPE_OBJECT]   = new ArrayType(env, jniEnv, TYPE_OBJECT,   "ObjectArray",  "(J)V", "(I)Ljava/lang/Object;", "(ILjava/lang/Object;)V");
  interfaces = TArray<ArrayType*>::New();
}

ArrayConv::~ArrayConv() {}

void ArrayConv::dispose(JNIEnv *jniEnv) {
  sArrayHiddenKey.Dispose();
  sLength.Dispose();
  for(int i = 0; i < TYPE___END; i++) if(typeToArray[i]) typeToArray[i]->dispose(jniEnv);
  for(int i = 0; i < interfaces->getLength(); i++) interfaces->get(i)->dispose(jniEnv);
}

int ArrayConv::WrapV8Array(JNIEnv *jniEnv, Handle<Array> val, jobject *jVal) {
  Persistent<Array> pVal = Persistent<Array>::New(val);
  ArrayType *objectRef = typeToArray[TYPE_OBJECT];
  jobject ob = jniEnv->NewObject(objectRef->js.class_, objectRef->js.ctor, asLong(pVal));
  return conv->BindToV8Object(jniEnv, val, objectRef->sHiddenKey, ob, jVal);
}

int ArrayConv::GetArrayType(JNIEnv *jniEnv, classId class_, ArrayType **ref) {
  int result = OK;
  int idx = class_ >> 1;
  *ref = interfaces->get(idx);
  if(!ref) {
    interfaces->put(idx, (*ref = new ArrayType(env, jniEnv, getInterfaceType(class_), "ObjectArray", "(J)V", "(I)Ljava/lang/Object;", "(ILjava/lang/Object;)V")));
    result = *ref ? OK : ErrorMem;
  }
  return result;
}

int ArrayConv::ToJavaArray(JNIEnv *jniEnv, Handle<Value> val, int componentType, jobject *jVal) {
  if(val.IsEmpty()) return ErrorNotfound;
  if(!val->IsObject()) return ErrorType;

  ArrayType *arr;
  Handle<Object> oVal = Handle<Object>::Cast(val);
  int result = GetRefsForComponentType(jniEnv, componentType, &arr);
  if(result == OK) {
    result = conv->UnwrapObject(jniEnv, oVal, arr->sHiddenKey, jVal);
    if(result == ErrorNotfound)
      result = WrapV8Array(jniEnv, oVal, componentType, arr, jVal);
  }
  return result;
}

int ArrayConv::WrapV8Array(JNIEnv *jniEnv, Handle<Object> val, int componentType, ArrayType *arr, jobject *jVal) {
  Persistent<Object> pVal = Persistent<Object>::New(val);
  jobject ob;
  if(arr->UserNew(jniEnv, &ob) == OK) {
    return conv->BindToV8Object(jniEnv, val, arr->sHiddenKey, ob, jVal);
  }
  if(jniEnv->ExceptionCheck())
    jniEnv->ExceptionClear();
  return ErrorVM;
}

int ArrayConv::ToV8Array(JNIEnv *jniEnv, jobject jVal, int expectedType, Handle<Object> *val) {
  int result = conv->UnwrapObject(jniEnv, jVal, val);
  if(result == ErrorNotfound) {
    int componentType = getComponentType(expectedType);
    ArrayType *arr;
    result = GetRefsForComponentType(jniEnv, componentType, &arr);
    if(result == OK) {
      Handle<Object> vInst;
      result = arr->PlatformNew(jniEnv, &vInst);
      if(result == OK) {
        result = conv->BindToJavaObject(jniEnv, jVal, vInst, arr->sHiddenKey);
        if(result == OK)
          *val = vInst;
      }
    }
  }
  return result;
}

int ArrayConv::UserGetLength(JNIEnv *jniEnv, Handle<Object> val, int *length) {
  HandleScope scope;
  TryCatch tryCatch;
  Handle<Value> vLength = val->Get(sLength);
  if(vLength.IsEmpty()) return ErrorNotfound;
  if(tryCatch.HasCaught()) return ErrorJS;
  return (int)val->IntegerValue();
}

int ArrayConv::UserSetLength(JNIEnv *jniEnv, Handle<Object> val, int length) {
  TryCatch tryCatch;
  val->Set(sLength, Handle<Value>(Integer::New(length)));
  if(tryCatch.HasCaught()) return ErrorJS;
  return OK;
}

int ArrayConv::UserGetElement(JNIEnv *jniEnv, Handle<Object> val, unsigned int type, int idx, jobject *jVal) {
  HandleScope scope;
  TryCatch tryCatch;
  Handle<Value> vElement = val->Get(idx);
  if(vElement.IsEmpty()) return ErrorNotfound;
  if(tryCatch.HasCaught()) return ErrorJS;
  return conv->ToJavaObject(jniEnv, vElement, getComponentType((unsigned int)type), jVal);
}

int ArrayConv::UserSetElement(JNIEnv *jniEnv, Handle<Object> val, unsigned int type, int idx, jobject jVal) {
  HandleScope scope;
  TryCatch tryCatch;
  Handle<Value> vElement;
  int result = conv->ToV8Value(jniEnv, jVal, getComponentType((unsigned int)type), &vElement);
  if(result == OK) {
    val->Set(idx, vElement);
    if(tryCatch.HasCaught()) result =  ErrorJS;
  }
  return result;
}

int ArrayConv::GetRefsForComponentType(JNIEnv *jniEnv, unsigned int componentType, ArrayType **ref) {
  int result = OK;
  if(isInterfaceOrDict(componentType)) {
    result = GetArrayType(jniEnv, getClassId(componentType), ref);
  } else {
    *ref = typeToArray[componentType];
    result = *ref ? OK : ErrorType;
  }
  return result;
}