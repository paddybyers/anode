#include "V8ToJava.h"
#include <math.h>

V8ToJava::V8ToJava(JNIEnv *jniEnv) {
  sRefHiddenKey = Persistent<String>::New(String::NewSymbol("node::GlobalRef"));
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

  jni.anode.js.JSObject.ctor         = jniEnv->GetMethodID(jni.anode.js.JSObject.class_,             "<init>",      "(J)V");
  jni.anode.js.JSValue_Bool.ctor     = jniEnv->GetStaticMethodID(jni.anode.js.JSValue_Bool.class_,   "asJSBoolean", "(Z)Lorg/meshpoint/anode/js/JSValue;");
  jni.anode.js.JSValue_Long.ctor     = jniEnv->GetStaticMethodID(jni.anode.js.JSValue_Long.class_,   "asJSNumber",  "(J)Lorg/meshpoint/anode/js/JSValue;");
  jni.anode.js.JSValue_Double.ctor   = jniEnv->GetStaticMethodID(jni.anode.js.JSValue_Double.class_, "asJSNumber",  "(D)Lorg/meshpoint/anode/js/JSValue;");
}

V8ToJava::~V8ToJava() {}

void V8ToJava::dispose(JNIEnv *jniEnv) {
  sRefHiddenKey.Dispose();
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

int V8ToJava::GetType(Handle<Value> val) {

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

int V8ToJava::ToJavaObject(JNIEnv *jniEnv, Handle<Value> val, jobject *jVal) {
  jobject ob;
  switch(GetType(val)) {
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
    case TYPE_FUNCTION:
    case TYPE_ARRAY:
    case TYPE_OBJECT:
      return ToJavaObject(jniEnv, val->ToObject(), jVal);
  }
  if(ob) {
    *jVal = ob;
    return OK;
  }
  if(jniEnv->ExceptionCheck())
    jniEnv->ExceptionClear();
  return ErrorVM;
}

int V8ToJava::ToJavaString(JNIEnv *jniEnv, Handle<String> val, jstring *jVal) {
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

int V8ToJava::ToJavaString(JNIEnv *jniEnv, const char *valUtf, jstring *jVal) {
  jstring ob = jniEnv->NewStringUTF(valUtf);
  if(ob) {
    *jVal = ob;
    return OK;
  }
  if(jniEnv->ExceptionCheck())
    jniEnv->ExceptionClear();
  return ErrorVM;
}


int V8ToJava::ToJavaString(JNIEnv *jniEnv, Handle<Value> val, jstring *jVal) {
  HandleScope scope;
  Handle<String> vString;
  Handle<Value> empty, vRes;
  jstring ob;
  char buf[64];
  switch(GetType(val)) {
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

int V8ToJava::ToJavaObject(JNIEnv *jniEnv, Handle<Object> val, int expectedType, jobject *jVal) {
  HandleScope scope;

  /* inspect value of hidden field */
  Local<Value> refValue = val->GetHiddenValue(sRefHiddenKey);
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
      val->SetHiddenValue(sRefHiddenKey, External::Wrap((void *)wrapperRef));
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

int V8ToJava::ToJavaObject(JNIEnv *jniEnv, Handle<Value> val, int expectedType, jobject *jVal) {
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

int V8ToJava::ToJavaDate(JNIEnv *jniEnv, Handle<Object> val, jobject *jVal) {
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

int V8ToJava::ToJavaArray(JNIEnv *jniEnv, Handle<Object> val, int componentType, jarray *jVal) {
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

