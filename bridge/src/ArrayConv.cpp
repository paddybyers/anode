#include "ArrayConv.h"
#include "Env.h"
#include "VM.h"
#include <string.h>
#include <math.h>

using namespace v8;
using namespace bridge;

ArrayType::ArrayType(Env *env,
                     JNIEnv *jniEnv,
                     unsigned int componentType,
                     const char *ClassName,
                     const char *jsCtor,
                     const char *javaGetterSig,
                     const char *javaSetterSig,
                     eltGetter getter,
                     eltSetter setter)
{
  this->env           = env;
  this->componentType = componentType;
  char nameBuf[64];
  sprintf(nameBuf, "org/meshpoint/anode/js/JS%s", ClassName);
  js.class_   = (jclass)jniEnv->NewGlobalRef(jniEnv->FindClass(nameBuf));
  sprintf(nameBuf, "org/meshpoint/anode/java/%s", ClassName);
  java.class_ = (jclass)jniEnv->NewGlobalRef(jniEnv->FindClass(nameBuf));
  js.ctor     = jniEnv->GetMethodID(js.class_,   "<init>",     jsCtor);
  getLength   = jniEnv->GetMethodID(java.class_, "getLength",  "()I");
  setLength   = jniEnv->GetMethodID(java.class_, "setLength",  "(I)V");
  getElement  = jniEnv->GetMethodID(java.class_, "getElement", javaGetterSig);
  setElement  = jniEnv->GetMethodID(java.class_, "setElement", javaSetterSig);
  this->getter = getter;
  this->setter = setter;
  sHiddenKey  = Persistent<String>::New(Conv::getTypeKey(TYPE_ARRAY|componentType));
  sClassName  = Persistent<String>::New(String::New(ClassName));
}

int ArrayType::UserNew(JNIEnv *jniEnv, jlong handle, jobject *jVal) {
  if(isJavaObject(componentType))
    *jVal = jniEnv->NewObject(js.class_, js.ctor, handle, componentType);
  else
    *jVal = jniEnv->NewObject(js.class_, js.ctor, handle);
  return *jVal ? OK : ErrorVM;
}

Handle<Value> ArrayType::PlatformCtor(const Arguments& args) {
  return args.This();
}

int ArrayType::PlatformNew(JNIEnv *jniEnv, Handle<Object> *val) {
  int result = OK;
  if(function.IsEmpty()) {
    functionTemplate = Persistent<FunctionTemplate>::New(FunctionTemplate::New(PlatformCtor));
    functionTemplate->SetClassName(sClassName);
    Local<ObjectTemplate> instanceTemplate = functionTemplate->InstanceTemplate();
    instanceTemplate->SetInternalFieldCount(2);
    instanceTemplate->SetAccessor(env->getConv()->getArrayConv()->getSLength(), PlatformLengthGet, PlatformLengthSet);
    instanceTemplate->SetIndexedPropertyHandler(PlatformElementGet, PlatformElementSet);
    function = Persistent<Function>::New(functionTemplate->GetFunction());
    /* set prototype to inherit from Array */
    function->Set(String::New("prototype"), Array::New()->GetPrototype());
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

Handle<Value> ArrayType::PlatformLengthGet(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  jobject ob = (jobject)info.This()->GetPointerFromInternalField(0);
  ArrayType *arr = (ArrayType *)info.This()->GetPointerFromInternalField(1);
  JNIEnv *jniEnv = arr->env->getVM()->getJNIEnv();
  jint length = jniEnv->CallIntMethod(ob, arr->getLength);
  if(arr->env->getConv()->CheckForException(jniEnv)) {
    return Undefined();
  }
  return scope.Close(Integer::New(length));
}

void ArrayType::PlatformLengthSet(Local<String> property, Local<Value> value, const AccessorInfo& info) {
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
  arr->env->getConv()->CheckForException(jniEnv);
}

void ArrayType::DoubleSet(ArrayType *arr, JNIEnv *jniEnv, jobject ob, uint32_t index, v8::Handle<v8::Value> elt) {
  jniEnv->CallVoidMethod(ob, arr->setElement, index, (jdouble)elt->NumberValue());
}

Handle<Value> ArrayType::PlatformElementGet(uint32_t index, const AccessorInfo& info) {
  HandleScope scope;
  jobject ob = (jobject)info.This()->GetPointerFromInternalField(0);
  ArrayType *arr = (ArrayType *)info.This()->GetPointerFromInternalField(1);
  JNIEnv *jniEnv = arr->env->getVM()->getJNIEnv();
  /* use simple getter for primitive types */
  if(arr->getter) return scope.Close((arr->getter)(arr, jniEnv, ob, index));
  /* else do object call */
  jobject jVal = jniEnv->CallObjectMethod(ob, arr->getElement, index);
  if(arr->env->getConv()->CheckForException(jniEnv)) {
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

Handle<Value> ArrayType::PlatformElementSet(uint32_t index, Local<Value> value, const AccessorInfo& info) {
  HandleScope scope;
  jobject ob = (jobject)info.This()->GetPointerFromInternalField(0);
  ArrayType *arr = (ArrayType *)info.This()->GetPointerFromInternalField(1);
  JNIEnv *jniEnv = arr->env->getVM()->getJNIEnv();
  /* use simple setter for primitive types */
  if(arr->setter) {
    (arr->setter)(arr, jniEnv, ob, index, value);
    return value;
  }
  /* else do object call */
  jobject jVal;
  int result = arr->env->getConv()->ToJavaObject(jniEnv, value, arr->componentType, &jVal);
  if(result == OK) {
    jniEnv->CallVoidMethod(ob, arr->setElement, index, jVal);
    if(arr->env->getConv()->CheckForException(jniEnv)) {
      return Undefined();
    }
  }
  if(result != OK)
    Conv::ThrowV8ExceptionForErrno(result);

  return scope.Close(value);
}

Handle<Value> ArrayType::ByteGet(ArrayType *arr, JNIEnv *jniEnv, jobject ob, uint32_t index) {
  return Handle<Value>(Number::New(jniEnv->CallByteMethod(ob, arr->getElement, index)));
}

Handle<Value> ArrayType::IntegerGet(ArrayType *arr, JNIEnv *jniEnv, jobject ob, uint32_t index) {
  return Handle<Value>(Number::New(jniEnv->CallIntMethod(ob, arr->getElement, index)));
}

Handle<Value> ArrayType::LongGet(ArrayType *arr, JNIEnv *jniEnv, jobject ob, uint32_t index) {
  return Handle<Value>(Number::New(jniEnv->CallLongMethod(ob, arr->getElement, index)));
}

Handle<Value> ArrayType::DoubleGet(ArrayType *arr, JNIEnv *jniEnv, jobject ob, uint32_t index) {
  return Handle<Value>(Number::New(jniEnv->CallDoubleMethod(ob, arr->getElement, index)));
}

void ArrayType::ByteSet(ArrayType *arr, JNIEnv *jniEnv, jobject ob, uint32_t index, v8::Handle<v8::Value> elt) {
  jniEnv->CallVoidMethod(ob, arr->setElement, index, (jbyte)elt->Int32Value());
}

void ArrayType::IntegerSet(ArrayType *arr, JNIEnv *jniEnv, jobject ob, uint32_t index, v8::Handle<v8::Value> elt) {
  jniEnv->CallVoidMethod(ob, arr->setElement, index, (jint)elt->Int32Value());
}

void ArrayType::LongSet(ArrayType *arr, JNIEnv *jniEnv, jobject ob, uint32_t index, v8::Handle<v8::Value> elt) {
  jniEnv->CallVoidMethod(ob, arr->setElement, index, (jlong)elt->IntegerValue());
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
  typeToArray[TYPE_BYTE]     = new ArrayType(env, jniEnv, TYPE_BYTE,     "ByteArray",    "(J)V", "(I)B", "(IB)V", ArrayType::ByteGet, ArrayType::ByteSet);
  typeToArray[TYPE_INT]      = new ArrayType(env, jniEnv, TYPE_INT,      "IntegerArray", "(J)V", "(I)I", "(II)V", ArrayType::IntegerGet, ArrayType::IntegerSet);
  typeToArray[TYPE_LONG]     = new ArrayType(env, jniEnv, TYPE_LONG,     "LongArray",    "(J)V", "(I)J", "(IJ)V", ArrayType::LongGet, ArrayType::LongSet);
  typeToArray[TYPE_DOUBLE]   = new ArrayType(env, jniEnv, TYPE_DOUBLE,   "DoubleArray",  "(J)V", "(I)D", "(ID)V", ArrayType::DoubleGet, ArrayType::DoubleSet);
  typeToArray[TYPE_STRING]   = new ArrayType(env, jniEnv, TYPE_STRING,   "ObjectArray",  "(JI)V", "(I)Ljava/lang/Object;", "(ILjava/lang/Object;)V");
  typeToArray[TYPE_MAP]      = new ArrayType(env, jniEnv, TYPE_MAP,      "ObjectArray",  "(JI)V", "(I)Ljava/lang/Object;", "(ILjava/lang/Object;)V");
  typeToArray[TYPE_FUNCTION] = new ArrayType(env, jniEnv, TYPE_FUNCTION, "ObjectArray",  "(JI)V", "(I)Ljava/lang/Object;", "(ILjava/lang/Object;)V");
  typeToArray[TYPE_DATE]     = new ArrayType(env, jniEnv, TYPE_DATE,     "ObjectArray",  "(JI)V", "(I)Ljava/lang/Object;", "(ILjava/lang/Object;)V");
  typeToArray[TYPE_OBJECT]   = new ArrayType(env, jniEnv, TYPE_OBJECT,   "ObjectArray",  "(JI)V", "(I)Ljava/lang/Object;", "(ILjava/lang/Object;)V");

  interfaces = TArray<ArrayType*>::New();
  
  if(jniEnv->ExceptionCheck()) {
    LOGV("Env::ArrayConv(): JNI error\n");
    jniEnv->ExceptionClear();
  }
}

ArrayConv::~ArrayConv() {}

void ArrayConv::dispose(JNIEnv *jniEnv) {
  sArrayHiddenKey.Dispose();
  sLength.Dispose();
  for(int i = 0; i < TYPE___END; i++) if(typeToArray[i])
    if(typeToArray[i]) typeToArray[i]->dispose(jniEnv);

  for(int i = 0; i < interfaces->getLength(); i++)
    if(interfaces->get(i)) interfaces->get(i)->dispose(jniEnv);
}

int ArrayConv::WrapV8Array(JNIEnv *jniEnv, Handle<Array> val, jobject *jVal) {
  Persistent<Array> pVal = Persistent<Array>::New(val);
  ArrayType *objectRef = typeToArray[TYPE_OBJECT];
  jobject ob = jniEnv->NewObject(objectRef->js.class_, objectRef->js.ctor, asLong(pVal));
  return conv->BindToV8Object(jniEnv, val, objectRef->sHiddenKey, ob, jVal);
}

int ArrayConv::GetArrayType(JNIEnv *jniEnv, classId class_, ArrayType **ref) {
  int result = OK;
  int idx = Interface::classId2Idx(class_);
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
  if(arr->UserNew(jniEnv, asLong(pVal), &ob) == OK) {
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
  *length = (int)vLength->IntegerValue();
  return OK;
}

int ArrayConv::UserSetLength(JNIEnv *jniEnv, Handle<Object> val, int length) {
  TryCatch tryCatch;
  val->Set(sLength, Handle<Value>(Integer::New(length)));
  if(tryCatch.HasCaught()) return ErrorJS;
  return OK;
}

int ArrayConv::UserGetElement(JNIEnv *jniEnv, Handle<Object> val, unsigned int elementType, int idx, jobject *jVal) {
  HandleScope scope;
  TryCatch tryCatch;
  Handle<Value> vElement = val->Get(idx);
  if(vElement.IsEmpty()) return ErrorNotfound;
  if(tryCatch.HasCaught()) return ErrorJS;
  return conv->ToJavaObject(jniEnv, vElement, elementType, jVal);
}

int ArrayConv::UserSetElement(JNIEnv *jniEnv, Handle<Object> val, unsigned int elementType, int idx, jobject jVal) {
  HandleScope scope;
  TryCatch tryCatch;
  Handle<Value> vElement;
  int result = conv->ToV8Value(jniEnv, jVal, elementType, &vElement);
  if(result == OK) {
    val->Set(idx, vElement);
    if(tryCatch.HasCaught()) result =  ErrorJS;
  }
  return result;
}

int ArrayConv::GetRefsForComponentType(JNIEnv *jniEnv, unsigned int elementType, ArrayType **ref) {
  int result = OK;
  if(isInterfaceOrDict(elementType)) {
    result = GetArrayType(jniEnv, getClassId(elementType), ref);
  } else {
    *ref = typeToArray[elementType];
    result = *ref ? OK : ErrorType;
  }
  return result;
}