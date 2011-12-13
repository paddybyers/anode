#include "Conv.h"
#include "Env.h"
#include <string.h>
#include <math.h>

Conv::Conv(Env *env, JNIEnv *jniEnv) {
  this->env = env;

  sObjectHiddenKey = Persistent<String>::New(String::NewSymbol("node::GlobalRef"));
  sToString     = Persistent<String>::New(String::NewSymbol("toString"));
  sLength       = Persistent<String>::New(String::NewSymbol("length"));
  
  jni.java.lang.Boolean.class_       = (jclass)jniEnv->NewGlobalRef(jniEnv->FindClass("java/lang/Boolean"));
  jni.java.lang.Byte.class_          = (jclass)jniEnv->NewGlobalRef(jniEnv->FindClass("java/lang/Byte"));
  jni.java.lang.Integer.class_       = (jclass)jniEnv->NewGlobalRef(jniEnv->FindClass("java/lang/Integer"));
  jni.java.lang.Long.class_          = (jclass)jniEnv->NewGlobalRef(jniEnv->FindClass("java/lang/Long"));
  jni.java.lang.Double.class_        = (jclass)jniEnv->NewGlobalRef(jniEnv->FindClass("java/lang/Double"));
  jni.java.lang.String.class_        = (jclass)jniEnv->NewGlobalRef(jniEnv->FindClass("java/lang/String"));
  jni.java.lang.Object.class_        = (jclass)jniEnv->NewGlobalRef(jniEnv->FindClass("java/lang/Object"));
  jni.java.util.Date.class_          = (jclass)jniEnv->NewGlobalRef(jniEnv->FindClass("java/util/Date"));
  
  jni.java.lang.Boolean.ctor         = jniEnv->GetMethodID(jni.java.lang.Boolean.class_, "<init>", "(Z)V");
  jni.java.lang.Integer.ctor         = jniEnv->GetMethodID(jni.java.lang.Integer.class_, "<init>", "(I)V");
  jni.java.lang.Long.ctor            = jniEnv->GetMethodID(jni.java.lang.Long.class_,    "<init>", "(J)V");
  jni.java.lang.Double.ctor          = jniEnv->GetMethodID(jni.java.lang.Double.class_,  "<init>", "(D)V");
  jni.java.util.Date.ctor            = jniEnv->GetMethodID(jni.java.util.Date.class_,    "<init>", "(J)V");
  
  jni.anode.js.JSValue_Bool.class_   =
  jni.anode.js.JSValue_Long.class_   =
  jni.anode.js.JSValue_Double.class_ = (jclass)jniEnv->NewGlobalRef(jniEnv->FindClass("org/meshpoint/anode/js/JSValue"));
  jni.anode.js.JSObject.class_       = (jclass)jniEnv->NewGlobalRef(jniEnv->FindClass("org/meshpoint/anode/js/JSObject"));
  jni.anode.js.JSFunction.class_     = (jclass)jniEnv->NewGlobalRef(jniEnv->FindClass("org/meshpoint/anode/js/JSFunction"));
  jni.anode.js.JSArray.class_        = (jclass)jniEnv->NewGlobalRef(jniEnv->FindClass("org/meshpoint/anode/js/JSArray"));
  jni.anode.js.JSInterface.class_    = (jclass)jniEnv->NewGlobalRef(jniEnv->FindClass("org/meshpoint/anode/js/JSInterface"));

  jni.anode.js.JSObject.ctor         = jniEnv->GetMethodID(jni.anode.js.JSObject.class_,             "<init>",      "(J)V");
  jni.anode.js.JSFunction.ctor       = jniEnv->GetMethodID(jni.anode.js.JSFunction.class_,           "<init>",      "(J)V");
  jni.anode.js.JSArray.ctor          = jniEnv->GetMethodID(jni.anode.js.JSArray.class_,              "<init>",      "(J)V");
  jni.anode.js.JSInterface.ctor      = jniEnv->GetMethodID(jni.anode.js.JSInterface.class_,          "<init>",      "(JLorg/meshpoint/anode/idl/IDLInterface;)V");
  jni.anode.js.JSValue_Bool.ctor     = jniEnv->GetStaticMethodID(jni.anode.js.JSValue_Bool.class_,   "asJSBoolean", "(Z)Lorg/meshpoint/anode/js/JSValue;");
  jni.anode.js.JSValue_Long.ctor     = jniEnv->GetStaticMethodID(jni.anode.js.JSValue_Long.class_,   "asJSNumber",  "(J)Lorg/meshpoint/anode/js/JSValue;");
  jni.anode.js.JSValue_Double.ctor   = jniEnv->GetStaticMethodID(jni.anode.js.JSValue_Double.class_, "asJSNumber",  "(D)Lorg/meshpoint/anode/js/JSValue;");
  
  jni.java.lang.Boolean.getter       = jniEnv->GetMethodID(jni.java.lang.Boolean.class_,       "booleanValue",    "()Z");
  jni.java.lang.Byte.getter          = jniEnv->GetMethodID(jni.java.lang.Byte.class_,          "byteValue",       "()B");
  jni.java.lang.Integer.getter       = jniEnv->GetMethodID(jni.java.lang.Integer.class_,       "intValue",        "()I");
  jni.java.lang.Long.getter          = jniEnv->GetMethodID(jni.java.lang.Long.class_,          "longValue",       "()J");
  jni.java.lang.Double.getter        = jniEnv->GetMethodID(jni.java.lang.Double.class_,        "doubleValue",     "()I");
  jni.anode.js.JSValue_Bool.getter   = jniEnv->GetMethodID(jni.anode.js.JSValue_Bool.class_,   "getBooleanValue", "()Z");
  jni.anode.js.JSValue_Long.getter   = jniEnv->GetMethodID(jni.anode.js.JSValue_Long.class_,   "getLongValue",    "()J");
  jni.anode.js.JSValue_Double.getter = jniEnv->GetMethodID(jni.anode.js.JSValue_Double.class_, "getDoubleValue",  "()D");

  memset(&typeToRef, 0, sizeof(typeToRef));
  typeToRef[TYPE_BOOL]   = &jni.java.lang.Boolean;
  typeToRef[TYPE_BYTE]   = &jni.java.lang.Byte;
  typeToRef[TYPE_INT]    = &jni.java.lang.Integer;
  typeToRef[TYPE_LONG]   = &jni.java.lang.Long;
  typeToRef[TYPE_DOUBLE] = &jni.java.lang.Double;
  typeToRef[TYPE_STRING] = &jni.java.lang.String;
  typeToRef[TYPE_DATE]   = &jni.java.util.Date;
  typeToRef[TYPE_OBJECT] = &jni.java.lang.Object;
  
  classClass = jniEnv->FindClass("java/lang/Class");
  classIsArray = jniEnv->GetMethodID(classClass, "isArray", "()Z");
  classGetComponentType = jniEnv->GetMethodID(classClass, "getComponentType", "()Ljava/lang/Class;");
}

Conv::~Conv() {}

void Conv::dispose(JNIEnv *jniEnv) {
  sObjectHiddenKey.Dispose();
  sToString.Dispose();
  sLength.Dispose();
  jniEnv->DeleteGlobalRef(jni.java.lang.Boolean.class_);
  jniEnv->DeleteGlobalRef(jni.java.lang.Byte.class_);
  jniEnv->DeleteGlobalRef(jni.java.lang.Integer.class_);
  jniEnv->DeleteGlobalRef(jni.java.lang.Long.class_);
  jniEnv->DeleteGlobalRef(jni.java.lang.Double.class_);
  jniEnv->DeleteGlobalRef(jni.java.lang.String.class_);
  jniEnv->DeleteGlobalRef(jni.java.lang.Object.class_);
  jniEnv->DeleteGlobalRef(jni.java.util.Date.class_);
  jniEnv->DeleteGlobalRef(jni.anode.js.JSValue_Bool.class_);
  jniEnv->DeleteGlobalRef(jni.anode.js.JSObject.class_);
  jniEnv->DeleteGlobalRef(jni.anode.js.JSFunction.class_);
  jniEnv->DeleteGlobalRef(jni.anode.js.JSArray.class_);
}

/* convert a type number for a primitive type.
 * Does not support array or interface types atm */
jclass Conv::type2Class(int type) {
  if(type & (TYPE_ARRAY|TYPE_INTERFACE))
    return 0;
  if(type > TYPE_OBJECT) type &= ~TYPE_OBJECT;
  return typeToRef[type]->class_;
}

/* attempt to resolve a java class to a type
 * Must be given a global reference to the class */
int Conv::class2Type(JNIEnv *jniEnv, jclass class_, jclass *componentType) {
  int result = TYPE_NONE;
  for(int i = 0; i < TYPE___END; i++) {
    if(jniEnv->IsSameObject(typeToRef[i]->class_, class_)) {
      result = (i | TYPE_OBJECT);
      break;
    }
  }
  if(result == TYPE_NONE) {
    if(jniEnv->CallBooleanMethod(class_, classIsArray)) {
      jclass componentClass = (jclass)jniEnv->CallObjectMethod(class_, classGetComponentType);
      result = class2Type(jniEnv, componentClass);
      if(componentType && componentClass && result != TYPE_NONE)
        *componentType = componentClass;
    }
  }
  return result;
}

/* attempt to resolve a java class to a type
 * Must be given a global reference to the class */
int Conv::ob2Type(JNIEnv *jniEnv, jobject ob, jclass *componentType) {
  return class2Type(jniEnv, jniEnv->GetObjectClass(ob), componentType);
}

int Conv::GetNaturalType(Handle<Value> val) {

  if(val.IsEmpty()) return TYPE_INVALID;
  if(val->IsUndefined()) return TYPE_UNDEFINED;
  if(val->IsNull()) return TYPE_NULL;
  if(val->IsBoolean() || val->IsBooleanObject()) return TYPE_BOOL;
  if(val->IsInt32()) return TYPE_INT;
  if(val->IsUint32()) return TYPE_LONG;
  if(val->IsNumber() || val->IsNumberObject()) return TYPE_DOUBLE;
  if(val->IsString() || val->IsStringObject()) return TYPE_STRING;
  if(val->IsDate()) return TYPE_DATE;
  if(val->IsFunction()) return TYPE_FUNCTION;
  if(val->IsArray()) return TYPE_ARRAY;
  if(val->IsObject()) return TYPE_OBJECT;
  return TYPE_INVALID;
}

/****************************
 * V8 to Java conversions
 ****************************/

int Conv::ToNaturalJavaObject(JNIEnv *jniEnv, Handle<Value> val, jobject *jVal) {
  jobject ob = 0;
  int result = 0;
  switch(GetNaturalType(val)) {
    default:
      return ErrorInvalid;
    case TYPE_UNDEFINED:
    case TYPE_NULL:
      *jVal = 0;
      return OK;
    case TYPE_BOOL:
      ob = jniEnv->NewObject(jni.java.lang.Boolean.class_, jni.java.lang.Boolean.ctor, val->BooleanValue());
      break;
    case TYPE_INT:
      ob = jniEnv->NewObject(jni.java.lang.Integer.class_, jni.java.lang.Integer.ctor, (jint)val->IntegerValue());
      break;
    case TYPE_LONG:
      ob = jniEnv->NewObject(jni.java.lang.Long.class_, jni.java.lang.Long.ctor, val->IntegerValue());
      break;
    case TYPE_DOUBLE:
      ob = jniEnv->NewObject(jni.java.lang.Double.class_, jni.java.lang.Double.ctor, val->NumberValue());
      break;
    case TYPE_STRING:
      return ToJavaString(jniEnv, val->ToString(), (jstring *)jVal);
      break;
    case TYPE_DATE:
      ob = jniEnv->NewObject(jni.java.util.Date.class_, jni.java.util.Date.ctor, val->NumberValue());
      break;
    case TYPE_FUNCTION: {
      Handle<Function> fVal = Handle<Function>(val->ToObject());
      result = UnwrapObject(jniEnv, fVal, jVal);
      if(result == ErrorNotfound)
        result = WrapV8Object(jniEnv, fVal, jVal);
      break;
    }
    case TYPE_ARRAY: {
      Handle<Array> aVal = Handle<Array>(val->ToObject());
      result = UnwrapObject(jniEnv, aVal, jVal);
      if(result == ErrorNotfound)
        result = WrapV8Object(jniEnv, aVal, jVal);
      break;
    }
    case TYPE_OBJECT: {
      Handle<Object> oVal = Handle<Object>(val->ToObject());
      result = UnwrapObject(jniEnv, oVal, jVal);
      if(result == ErrorNotfound)
        result = WrapV8Object(jniEnv, oVal, jVal);
      break;
    }
  }
  if(jniEnv->ExceptionCheck()) {
    jniEnv->ExceptionClear();
    return ErrorVM;
  }
  if(ob) {
    *jVal = ob;
    return OK;
  }
  return result;
}

int Conv::UnwrapInterface(JNIEnv *jniEnv, Handle<Object> val, classId class_, jobject *jVal) {
  Handle<String> sHiddenKey = env->getInterface(class_)->getHiddenKey();
  Local<Value> hiddenVal = val->GetHiddenValue(sHiddenKey);
  if(!hiddenVal.IsEmpty() && !hiddenVal->IsUndefined()) {
    *jVal = (jobject)External::Unwrap(hiddenVal);
    return OK;
  }
  return ErrorNotfound;
}

int Conv::UnwrapObject(JNIEnv *jniEnv, Handle<Object> val, jobject *jVal) {
  Local<Value> hiddenVal = val->GetHiddenValue(sObjectHiddenKey);
  if(!hiddenVal.IsEmpty() && !hiddenVal->IsUndefined()) {
    *jVal = (jobject)External::Unwrap(hiddenVal);
    return OK;
  }
  return ErrorNotfound;
}

int Conv::WrapV8Object(JNIEnv *jniEnv, Handle<Function> val, jobject *jVal) {
  Persistent<Function> pVal = Persistent<Function>::New(val);
  jobject ob = jniEnv->NewObject(jni.anode.js.JSFunction.class_, jni.anode.js.JSFunction.ctor, asLong(pVal));
  return BindToV8Object(jniEnv, val, sObjectHiddenKey, ob, jVal);
}

int Conv::WrapV8Object(JNIEnv *jniEnv, Handle<Array> val, jobject *jVal) {
  Persistent<Array> pVal = Persistent<Array>::New(val);
  jobject ob = jniEnv->NewObject(jni.anode.js.JSArray.class_, jni.anode.js.JSArray.ctor, asLong(pVal));
  return BindToV8Object(jniEnv, val, sObjectHiddenKey, ob, jVal);
}

int Conv::WrapV8Object(JNIEnv *jniEnv, Handle<Object> val, jobject *jVal) {
  Persistent<Object> pVal = Persistent<Object>::New(val);
  jobject ob = jniEnv->NewObject(jni.anode.js.JSObject.class_, jni.anode.js.JSObject.ctor, asLong(pVal));
  return BindToV8Object(jniEnv, val, sObjectHiddenKey, ob, jVal);
}

int Conv::WrapV8Interface(JNIEnv *jniEnv, Handle<Object> val, classId class_, jobject *jVal) {
  Persistent<Object> pVal = Persistent<Object>::New(val);
  Interface *interface = env->getInterface(class_);
  jobject ob;
  int result;
  if(isValueType(class_)) {
    
  } else {
    result = interface->CreateImport(jniEnv, asLong(pVal), &ob);
    if(result == OK) {
      result = BindToV8Object(jniEnv, val, interface->getHiddenKey(), ob, jVal);
    }
  }
  return result;
}

int Conv::BindToV8Object(JNIEnv *jniEnv, Handle<Object> val, Handle<String> key, jobject jLocal, jobject *jGlobal) {
  if(!jLocal || jniEnv->ExceptionCheck()) {
    jniEnv->ExceptionClear();
    return ErrorVM;
  }
  jobject ref = jniEnv->NewWeakGlobalRef(jLocal);
  val->SetHiddenValue(key, External::Wrap(ref));
  *jGlobal = ref;
  return OK;
}

int Conv::ToJavaObject(JNIEnv *jniEnv, Handle<Value> val, int expectedType, jobject *jVal) {
  return OK;
}

int Conv::ToJavaObject(JNIEnv *jniEnv, Handle<Object> val, int expectedType, jobject *jVal) {
  return OK;
}

int Conv::ToJavaSequence(JNIEnv *jniEnv, Handle<Object> val, int componentType, jarray *jVal) {
  return OK;
}

int Conv::ToJavaArray(JNIEnv *jniEnv, Handle<Object> val, int componentType, jobject *jVal) {
  return OK;
}

int Conv::ToJavaInterface(JNIEnv *jniEnv, Handle<Object> val, classId clsid, jobject *jVal) {
  return OK;
}

int Conv::ToJavaDate(JNIEnv *jniEnv, Handle<Object> val, jobject *jVal) {
  return OK;
}

int Conv::ToJavaValueType(JNIEnv *jniEnv, Handle<Object> val, jobject *jVal) {
  return OK;
}

int Conv::ToJavaString(JNIEnv *jniEnv, Handle<String> val, jstring *jVal) {
  int len = val->Length();
  jchar *buf = new jchar[len];
  if(!buf) return ErrorMem;
  val->Write(buf, 0, len, v8::String::NO_NULL_TERMINATION);
  jstring ob = jniEnv->NewString(buf, len);
  delete[] buf;
  if(ob) {
    *jVal = ob;
    return OK;
  }
  if(jniEnv->ExceptionCheck())
    jniEnv->ExceptionClear();
  return ErrorVM;
}

int Conv::ToJavaString(JNIEnv *jniEnv, const char *valUtf, jstring *jVal) {
  jstring ob = jniEnv->NewStringUTF(valUtf);
  if(ob) {
    *jVal = ob;
    return OK;
  }
  if(jniEnv->ExceptionCheck())
    jniEnv->ExceptionClear();
  return ErrorVM;
}


int Conv::ToJavaString(JNIEnv *jniEnv, Handle<Value> val, jstring *jVal) {
  HandleScope scope;
  Handle<String> vString;
  Handle<Value> empty, vRes;
  jstring ob;
  char buf[64];
  switch(GetNaturalType(val)) {
    default: {
      if(val->IsObject()) {
        /* call ToString() in javascript */
        Handle<Object> oVal = val->ToObject();
        Handle<Function> fToString = oVal->Get(sToString);
        if(!fToString.IsEmpty() && !fToString->IsUndefined()) {
          vRes = fToString->CallAsFunction(oVal, 0, &empty);
          if(!vRes.IsEmpty() && (vRes->IsString() || vRes->IsStringObject())) {
            return ToJavaString(jniEnv, vRes->ToString(), jVal);
          }
        }
      }
      return ErrorType;
    }
    case TYPE_UNDEFINED:
    case TYPE_NULL:
      *jVal = 0;
      return OK;
    case TYPE_BOOL:
      return ToJavaString(jniEnv, (val->BooleanValue() ? "true" : "false"), jVal);
    case TYPE_INT:
      sprintf(buf, "%d", val->Int32Value());
      return ToJavaString(jniEnv, buf, jVal);
    case TYPE_LONG:
      sprintf(buf, "%lld", val->IntegerValue());
      return ToJavaString(jniEnv, buf, jVal);
    case TYPE_DOUBLE:
      sprintf(buf, "%g", val->NumberValue());
      return ToJavaString(jniEnv, buf, jVal);
    case TYPE_STRING:
      return ToJavaString(jniEnv, val->ToString(), (jstring *)jVal);
  }
  if(ob) {
    *jVal = ob;
    return OK;
  }
  if(jniEnv->ExceptionCheck())
    jniEnv->ExceptionClear();
  return ErrorVM;
}

int Conv::ToJavaObject(JNIEnv *jniEnv, Handle<Object> val, int expectedType, jobject *jVal) {
  HandleScope scope;

  /* inspect value of hidden field */
  Local<Value> refValue = val->GetHiddenValue(sObjectHiddenKey);
  if(!refValue.IsEmpty()) {
    void *hiddenData = External::Unwrap(refValue);
    if(hiddenData) {
      /* ok, one of ours ... now see if the private field is there */
      if(val->InternalFieldCount() == PRIVATE_COUNT) {
        void *privateData = val->GetPointerFromInternalField(PRIVATE_GLOBALREF);
        if(privateData == hiddenData) {
          /* so it's a wrapped java object */
          *jVal = (jobject)hiddenData;
          return OK;
        }
      }
      /* so it must be a JS native object that was previously wrapped */
      jweak wrapperRef = (jweak)hiddenData;
      if(!jniEnv->IsSameObject(wrapperRef, 0)) {
        /* so the previously wrapped object is still there */
        *jVal = jniEnv->NewGlobalRef(wrapperRef);
        return OK;
      }
    }
  }
  
  /* if there's no expected type, or the expected type is pure object, wrap it */
  if(expectedType == TYPE_NONE || expectedType == TYPE_OBJECT) {
    Persistent<Object> persistRef = Persistent<Object>::New(val);
    jobject ob = jniEnv->NewObject(jni.anode.js.JSObject.class_, jni.anode.js.JSObject.ctor, asLong(persistRef));
    if(ob) {
      /* set up the hidden field for this wrapper */
      jweak wrapperRef = jniEnv->NewWeakGlobalRef(ob);
      val->SetHiddenValue(sObjectHiddenKey, External::Wrap((void *)wrapperRef));
      *jVal = ob;
      return OK;
    }
    /* ob construction failed, so dispose the reference */
    persistRef.Dispose();
  }
  
  if(jniEnv->ExceptionCheck())
    jniEnv->ExceptionClear();
  return ErrorVM;
}

int Conv::ToJavaObject(JNIEnv *jniEnv, Handle<Value> val, int expectedType, jobject *jVal) {
  if(isArray(expectedType))
    return ToJavaArray(jniEnv, val, getComponentType(expectedType), (jarray *)jVal);
    
  if(isInterface(expectedType))
    return ToJavaInterface(jniEnv, val, getClassId(expectedType), jVal);

  HandleScope scope;
  jobject ob;
  switch(expectedType) {
    default:
      return ErrorType;
    case TYPE_BOOL: {
      bool isVoid = val->BooleanValue();
      ob = jniEnv->CallStaticObjectMethod(jni.anode.js.JSValue_Bool.class_, jni.anode.js.JSValue_Bool.ctor, isVoid);
      break;
    }
    case TYPE_INT: {
      jint intValue = val->Int32Value();
      ob = jniEnv->CallStaticObjectMethod(jni.anode.js.JSValue_Long.class_, jni.anode.js.JSValue_Long.ctor, (jlong)intValue);
      break;
    }
    case TYPE_LONG: {
      jlong longValue = val->IntegerValue();
      ob = jniEnv->CallStaticObjectMethod(jni.anode.js.JSValue_Long.class_, jni.anode.js.JSValue_Long.ctor, longValue);
      break;
    }
    case TYPE_DOUBLE: {
      jdouble doubleValue = val->NumberValue();
      ob = jniEnv->CallStaticObjectMethod(jni.anode.js.JSValue_Double.class_, jni.anode.js.JSValue_Double.ctor, doubleValue);
      break;
    }
    case TYPE_STRING:
      return ToJavaString(jniEnv, val, (jstring *)jVal);
    case TYPE_DATE:
      return ToJavaDate(jniEnv, val, jVal);
    case TYPE_OBJECT:
      return ToJavaObject(jniEnv, val, TYPE_NONE, jVal);
    case TYPE_OBJECT|TYPE_BOOL: {
      bool isVoid = val->BooleanValue();
      ob = jniEnv->NewObject(jni.java.lang.Boolean.class_, jni.java.lang.Boolean.ctor, isVoid);
      break;
    }
    case TYPE_OBJECT|TYPE_INT: {
      jint intValue = val->Int32Value();
      ob = jniEnv->NewObject(jni.java.lang.Integer.class_, jni.java.lang.Integer.ctor, intValue);
      break;
    }
    case TYPE_OBJECT|TYPE_LONG: {
      jlong longValue = val->IntegerValue();
      ob = jniEnv->NewObject(jni.java.lang.Long.class_, jni.java.lang.Long.ctor, longValue);
      break;
    }
    case TYPE_OBJECT|TYPE_DOUBLE: {
      jdouble doubleValue = val->NumberValue();
      ob = jniEnv->NewObject(jni.java.lang.Double.class_, jni.java.lang.Double.ctor, doubleValue);
      break;
    }
  }
  if(ob) {
    *jVal = ob;
    return OK;
  }
  if(jniEnv->ExceptionCheck())
    jniEnv->ExceptionClear();
  return ErrorVM;
}

int Conv::ToJavaDate(JNIEnv *jniEnv, Handle<Object> val, jobject *jVal) {
  double dateTime;
  if(val->IsDate()) {
    dateTime = Date::Cast(*val)->NumberValue();
  } else {
    dateTime = val->NumberValue();
  }
  jobject ob = jniEnv->NewObject(jni.java.util.Date.class_, jni.java.util.Date.ctor, floor(dateTime));
  if(ob) {
    *jVal = ob;
    return OK;
  }
  if(jniEnv->ExceptionCheck())
    jniEnv->ExceptionClear();
  return ErrorVM;
}

int Conv::ToJavaSequence(JNIEnv *jniEnv, Handle<Object> val, int componentType, jarray *jVal) {
  HandleScope scope;
  Local<Object> oVal;
  Local<Value> vLength;
  if(val.IsEmpty() || val->IsNull() || val->IsUndefined()) {
    *jVal = 0;
    return OK;
  }
  if(!val->IsObject())
    return ErrorType;
  
  oVal = val->ToObject();
  vLength = val->Get(sLength);
  if(vLength.IsEmpty() || !vLength->IsNumber())
    return ErrorType;
  
  jint len = vLength->Uint32Value();

  if(isInterface(componentType) || componentType == TYPE_OBJECT) {
    /* this will be passed by reference (as Array<T> */
    /* TBD */
    return ErrorTBD;
  }
  
  jarray ob;
  switch(componentType) {
    default:
      return ErrorType;
    case TYPE_BYTE:
      ob = jniEnv->NewByteArray(len);
      if(ob) {
        /* FIXME: see if this case can be optimised */
        jbyte *buf = new jbyte[len];
        if(!buf) return ErrorMem;
        for(int i = 0; i < len; i++)
          buf[i] = (jbyte)val->Get(i)->Uint32Value();
        
        jniEnv->SetByteArrayRegion((jbyteArray)ob, 0, len, (const jbyte *)&buf);
        delete[] buf;
        break;
      }
    case TYPE_INT:
      ob = jniEnv->NewIntArray(len);
      if(ob) {
        jint *buf = new jint[len];
        if(!buf) return ErrorMem;
        for(int i = 0; i < len; i++)
          buf[i] = val->Get(i)->Int32Value();
        
        jniEnv->SetIntArrayRegion((jintArray)ob, 0, len, (const jint *)&buf);
        delete[] buf;
        break;
      }
    case TYPE_LONG:
      ob = jniEnv->NewLongArray(len);
      if(ob) {
        jlong *buf = new jlong[len];
        if(!buf) return ErrorMem;
        for(int i = 0; i < len; i++)
          buf[i] = val->Get(i)->IntegerValue();
        
        jniEnv->SetLongArrayRegion((jlongArray)ob, 0, len, (const jlong *)&buf);
        delete[] buf;
        break;
      }
    case TYPE_DOUBLE:
      ob = jniEnv->NewDoubleArray(len);
      if(ob) {
        jdouble *buf = new jdouble[len];
        if(!buf) return ErrorMem;
        for(int i = 0; i < len; i++)
          buf[i] = val->Get(i)->NumberValue();
        
        jniEnv->SetDoubleArrayRegion((jdoubleArray)ob, 0, len, (const jdouble *)&buf);
        delete[] buf;
        break;
      }
    case TYPE_STRING:
      ob = jniEnv->NewObjectArray(len, jni.java.lang.String.class_, 0);
      if(ob) {
        int res = OK;
        for(int i = 0; i < len; i++) {
          jstring item;
          res = ToJavaString(jniEnv, val->Get(i), &item);
          if(res != OK) break;
          jniEnv->SetObjectArrayElement((jobjectArray)ob, i, item);
        }
        break;
      }
    case TYPE_DATE:
      ob = jniEnv->NewObjectArray(len, jni.java.util.Date.class_, 0);
      if(ob) {
        int res = OK;
        for(int i = 0; i < len; i++) {
          jobject item;
          res = ToJavaDate(jniEnv, val->Get(i), &item);
          if(res != OK) break;
          jniEnv->SetObjectArrayElement((jobjectArray)ob, i, item);
        }
        break;
      }
  }
  if(ob) {
    *jVal = ob;
    return OK;
  }
  if(jniEnv->ExceptionCheck())
    jniEnv->ExceptionClear();
  return ErrorVM;
}

/****************************
 * Java to V8 conversions
 ****************************/

int Conv::ToV8Value(JNIEnv *jniEnv, jobject jVal, int expectedType, Handle<Value> *val) {
  HandleScope scope;
  int result = OK;
  if(expectedType == TYPE_NONE || expectedType == TYPE_OBJECT) {
    expectedType = ob2Type(jniEnv, jVal);
    if(expectedType == TYPE_NONE)
      return ErrorType;
  }

  if(isArray(expectedType)) {
    Handle<Array> arrayVal;
    result = ToV8Array(jniEnv, jVal, getComponentType(expectedType), &arrayVal);
    if(result == OK)
      *val = Handle<Value>(arrayVal);
    return result;
  }
  
  if(isInterface(expectedType)) {
    Handle<Object> objectVal;
    result = ToV8Interface(jniEnv, jVal, getComponentType(expectedType), &objectVal);
    if(result == OK)
      *val = Handle<Value>(objectVal);
    return result;
  }

  switch(expectedType) {
    default:
      return ErrorType;
    case TYPE_BOOL: {
      bool booleanValue = jniEnv->CallBooleanMethod(jVal, jni.anode.js.JSValue_Bool.getter);
      *val = Handle<Value>(Boolean::New(booleanValue));
      break;
    }
    case TYPE_INT: {
      jint intValue = (jint)jniEnv->CallLongMethod(jVal, jni.anode.js.JSValue_Long.getter);
      *val = Handle<Value>(Number::New(intValue));
      break;
    }
    case TYPE_LONG: {
      jlong longValue = jniEnv->CallLongMethod(jVal, jni.anode.js.JSValue_Long.getter);
      *val = Handle<Value>(Number::New(longValue));
      break;
    }
    case TYPE_DOUBLE: {
      jdouble doubleValue = jniEnv->CallDoubleMethod(jVal, jni.anode.js.JSValue_Double.getter);
      *val = Handle<Value>(Number::New(doubleValue));
      break;
    }
    case TYPE_STRING: {
      Handle<String> stringVal;
      result = ToV8String(jniEnv, (jstring)jVal, &stringVal);
      if(result == OK)
        *val = Handle<Value>(stringVal);
      break;
    }
    case TYPE_DATE: {
      Handle<Date> dateVal;
      result = ToV8Date(jniEnv, jVal, &dateVal);
      if(result == OK)
        *val = Handle<Value>(dateVal);
      break;
    }
    case TYPE_OBJECT: {
      Handle<Object> objectVal;
      result = ToV8Object(jniEnv, jVal, &objectVal);
      if(result == OK)
        *val = Handle<Value>(objectVal);
      break;
    }
    case TYPE_OBJECT|TYPE_BOOL: {
      bool booleanValue = jniEnv->CallBooleanMethod(jVal, jni.java.lang.Boolean.getter);
      *val = Handle<Value>(Boolean::New(booleanValue));
      break;
    }
    case TYPE_OBJECT|TYPE_INT: {
      jint intValue = jniEnv->CallIntMethod(jVal, jni.java.lang.Integer.getter);
      *val = Handle<Value>(Number::New(intValue));
      break;
    }
    case TYPE_OBJECT|TYPE_LONG: {
      jlong longValue = jniEnv->CallLongMethod(jVal, jni.java.lang.Long.getter);
      *val = Handle<Value>(Number::New(longValue));
      break; }
    case TYPE_OBJECT|TYPE_DOUBLE: {
      double doubleValue = jniEnv->CallDoubleMethod(jVal, jni.java.lang.Double.getter);
      *val = Handle<Value>(Number::New(doubleValue));
      break;
    }
  }
  if(jniEnv->ExceptionCheck()) {
    jniEnv->ExceptionClear();
    result = ErrorVM;
  }
  return result;
}
