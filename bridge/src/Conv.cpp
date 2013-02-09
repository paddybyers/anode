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

#include "Conv.h"
#include "ArrayConv.h"
#include "Env.h"
#include "VM.h"
#include <string.h>
#include <math.h>

using namespace v8;
using namespace bridge;

Conv::Conv(Env *env, JNIEnv *jniEnv) {
  LOGV("Conv::Conv(): ent: this = 0x%p, env = 0x%p\n", this, env);
  this->env = env;
  arrayConv = new ArrayConv(env, this, env->getVM()->getJNIEnv());

  sObjectHiddenKey = Persistent<String>::New(String::NewSymbol("node::object"));
  sToString        = Persistent<String>::New(String::NewSymbol("toString"));
  sLength          = Persistent<String>::New(String::NewSymbol("length"));

  jfieldID typeField;
  jni.java.lang.Boolean.class_       = (jclass)jniEnv->NewGlobalRef(jniEnv->FindClass("java/lang/Boolean"));
  typeField                          = jniEnv->GetStaticFieldID(jni.java.lang.Boolean.class_, "TYPE", "Ljava/lang/Class;");
  jni.java.lang.Boolean.primitive    = (jclass)jniEnv->NewGlobalRef(jniEnv->GetStaticObjectField(jni.java.lang.Boolean.class_,typeField));

  jni.java.lang.Byte.class_          = (jclass)jniEnv->NewGlobalRef(jniEnv->FindClass("java/lang/Byte"));
  typeField                          = jniEnv->GetStaticFieldID(jni.java.lang.Byte.class_, "TYPE", "Ljava/lang/Class;");
  jni.java.lang.Byte.primitive       = (jclass)jniEnv->NewGlobalRef(jniEnv->GetStaticObjectField(jni.java.lang.Byte.class_,typeField));

  jni.java.lang.Integer.class_       = (jclass)jniEnv->NewGlobalRef(jniEnv->FindClass("java/lang/Integer"));
  typeField                          = jniEnv->GetStaticFieldID(jni.java.lang.Integer.class_, "TYPE", "Ljava/lang/Class;");
  jni.java.lang.Integer.primitive    = (jclass)jniEnv->NewGlobalRef(jniEnv->GetStaticObjectField(jni.java.lang.Integer.class_,typeField));
  
  jni.java.lang.Long.class_          = (jclass)jniEnv->NewGlobalRef(jniEnv->FindClass("java/lang/Long"));
  typeField                          = jniEnv->GetStaticFieldID(jni.java.lang.Long.class_, "TYPE", "Ljava/lang/Class;");
  jni.java.lang.Long.primitive       = (jclass)jniEnv->NewGlobalRef(jniEnv->GetStaticObjectField(jni.java.lang.Long.class_,typeField));
  
  jni.java.lang.Double.class_        = (jclass)jniEnv->NewGlobalRef(jniEnv->FindClass("java/lang/Double"));
  typeField                          = jniEnv->GetStaticFieldID(jni.java.lang.Double.class_, "TYPE", "Ljava/lang/Class;");
  jni.java.lang.Double.primitive     = (jclass)jniEnv->NewGlobalRef(jniEnv->GetStaticObjectField(jni.java.lang.Double.class_,typeField));
  
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
  jni.anode.js.JSInterface.class_    = (jclass)jniEnv->NewGlobalRef(jniEnv->FindClass("org/meshpoint/anode/js/JSInterface"));

  jni.anode.js.JSObject.ctor         = jniEnv->GetMethodID(jni.anode.js.JSObject.class_,             "<init>",      "(J)V");
  jni.anode.js.JSFunction.ctor       = jniEnv->GetMethodID(jni.anode.js.JSFunction.class_,           "<init>",      "(J)V");
  jni.anode.js.JSInterface.ctor      = jniEnv->GetMethodID(jni.anode.js.JSInterface.class_,          "<init>",      "(J)V");
  jni.anode.js.JSValue_Bool.ctor     = jniEnv->GetStaticMethodID(jni.anode.js.JSValue_Bool.class_,   "asJSBoolean", "(Z)Lorg/meshpoint/anode/js/JSValue;");
  jni.anode.js.JSValue_Long.ctor     = jniEnv->GetStaticMethodID(jni.anode.js.JSValue_Long.class_,   "asJSNumber",  "(J)Lorg/meshpoint/anode/js/JSValue;");
  jni.anode.js.JSValue_Double.ctor   = jniEnv->GetStaticMethodID(jni.anode.js.JSValue_Double.class_, "asJSNumber",  "(D)Lorg/meshpoint/anode/js/JSValue;");
  
  jni.java.lang.Boolean.getter       = jniEnv->GetMethodID(jni.java.lang.Boolean.class_,       "booleanValue",    "()Z");
  jni.java.lang.Byte.getter          = jniEnv->GetMethodID(jni.java.lang.Byte.class_,          "byteValue",       "()B");
  jni.java.lang.Integer.getter       = jniEnv->GetMethodID(jni.java.lang.Integer.class_,       "intValue",        "()I");
  jni.java.lang.Long.getter          = jniEnv->GetMethodID(jni.java.lang.Long.class_,          "longValue",       "()J");
  jni.java.lang.Double.getter        = jniEnv->GetMethodID(jni.java.lang.Double.class_,        "doubleValue",     "()D");
  jni.java.util.Date.getter          = jniEnv->GetMethodID(jni.java.util.Date.class_,          "getTime",         "()J");
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
  
  classClass            = (jclass)jniEnv->NewGlobalRef(jniEnv->FindClass("java/lang/Class"));
  baseClass             = (jclass)jniEnv->NewGlobalRef(jniEnv->FindClass("org/meshpoint/anode/java/Base"));
  dictClass             = (jclass)jniEnv->NewGlobalRef(jniEnv->FindClass("org/meshpoint/anode/idl/Dictionary"));
  mapClass              = (jclass)jniEnv->NewGlobalRef(jniEnv->FindClass("java/util/HashMap"));
  setClass              = (jclass)jniEnv->NewGlobalRef(jniEnv->FindClass("java/util/Set"));
  classIsArray          = jniEnv->GetMethodID(classClass, "isArray", "()Z");
  classIsAssignableFrom = jniEnv->GetMethodID(classClass, "isAssignableFrom", "(Ljava/lang/Class;)Z");
  classIsPrimitive      = jniEnv->GetMethodID(classClass, "isPrimitive", "()Z");
  classGetComponentType = jniEnv->GetMethodID(classClass, "getComponentType", "()Ljava/lang/Class;");
  classGetName          = jniEnv->GetMethodID(classClass, "getName", "()Ljava/lang/String;");
  mapCtor               = jniEnv->GetMethodID(mapClass, "<init>", "()V");
  mapGet                = jniEnv->GetMethodID(mapClass, "get", "(Ljava/lang/Object;)Ljava/lang/Object;");
  mapPut                = jniEnv->GetMethodID(mapClass, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
  mapSize               = jniEnv->GetMethodID(mapClass, "size", "()I");
  mapKeySet             = jniEnv->GetMethodID(mapClass, "keySet", "()Ljava/util/Set;");
  setToArray            = jniEnv->GetMethodID(setClass, "toArray", "()[Ljava/lang/Object;");
  stringReplace         = jniEnv->GetMethodID(jni.java.lang.String.class_, "replace", "(CC)Ljava/lang/String;");
  objectToString        = jniEnv->GetMethodID(jni.java.lang.Object.class_, "toString", "()Ljava/lang/String;");
  instHandle            = jniEnv->GetFieldID(baseClass, "instHandle", "J");
  instType              = jniEnv->GetFieldID(baseClass, "type", "I");

  LOGV("Conv::Conv(): ret\n");
  if(jniEnv->ExceptionCheck()) {
    LOGV("Conv::Conv(): JNI error\n");
    jniEnv->ExceptionClear();
  }
}

Conv::~Conv() {
  delete arrayConv;
}

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
  
  jniEnv->DeleteGlobalRef(classClass);
  jniEnv->DeleteGlobalRef(baseClass);
  jniEnv->DeleteGlobalRef(dictClass);
  jniEnv->DeleteGlobalRef(mapClass);
  jniEnv->DeleteGlobalRef(setClass);
  
  arrayConv->dispose(jniEnv);
}

/* convert a type number for a primitive type.
 * Does not support array or interface types atm */
jclass Conv::type2Class(int type) {
  if(type & (TYPE_ARRAY|TYPE_IDL))
    return 0;
  if(type > TYPE_OBJECT) type &= ~TYPE_OBJECT;
  return typeToRef[type]->class_;
}

/* attempt to resolve a java class to a type
 * Must be given a global reference to the class */
int Conv::class2Type(JNIEnv *jniEnv, jclass class_, jclass *componentType) {
  int result = TYPE_INVALID;
  if(jniEnv->CallBooleanMethod(class_, classIsPrimitive)) {
    for(int i = 0; i < TYPE__OB_START; i++) {
      if(!typeToRef[i]) continue;
      if(jniEnv->IsSameObject(typeToRef[i]->primitive, class_)) {
        result = i;
        break;
      }
    }
    return result;
  }
  if(jniEnv->CallBooleanMethod(class_, classIsArray)) {
    jclass componentClass = (jclass)jniEnv->CallObjectMethod(class_, classGetComponentType);
    result = class2Type(jniEnv, componentClass);
    if(componentClass && result != TYPE_NONE) {
      if(componentType) *componentType = componentClass;
      result |= TYPE_SEQUENCE;
    }
    return result;
  }
  if(jniEnv->CallBooleanMethod(dictClass, classIsAssignableFrom, class_)) {
    return jniEnv->CallIntMethod(env->jEnv, env->findClassMethodId, class_);
  }

  for(int i = 0; i < TYPE___END; i++) {
    if(!typeToRef[i]) continue;
    if(jniEnv->IsSameObject(typeToRef[i]->class_, class_)) {
      result = i;
      if(!isJavaObject(i)) result |= TYPE_OBJECT;
      break;
    }
  }
  return result;
}

/* attempt to resolve a java class to a type
 * Must be given a global reference to the class */
int Conv::ob2Type(JNIEnv *jniEnv, jobject ob, jclass *componentType) {
  jclass class_ = jniEnv->GetObjectClass(ob);
  if(jniEnv->CallBooleanMethod(baseClass, classIsAssignableFrom, class_)) {
    return jniEnv->GetIntField(ob, instType);
  }
  return class2Type(jniEnv, class_, componentType);
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
  return GetNaturalType(Handle<Object>::Cast(val));
}

int Conv::GetNaturalType(Handle<Object> val) {
  if(val.IsEmpty()) return TYPE_INVALID;
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
  HandleScope scope;
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
      Handle<Function> fVal = Handle<Function>::Cast(val->ToObject());
      result = UnwrapObject(jniEnv, fVal, sObjectHiddenKey, jVal);
      if(result == ErrorNotfound)
        result = WrapV8Object(jniEnv, fVal, jVal);
      break;
    }
    case TYPE_ARRAY: {
      Handle<Array> aVal = Handle<Array>::Cast(val->ToObject());
      result = UnwrapObject(jniEnv, aVal, arrayConv->getHiddenKey(), jVal);
      if(result == ErrorNotfound)
        result = arrayConv->WrapV8Array(jniEnv, aVal, jVal);
      break;
    }
    case TYPE_OBJECT: {
      Handle<Object> oVal = Handle<Object>::Cast(val->ToObject());
      result = UnwrapObject(jniEnv, oVal, sObjectHiddenKey, jVal);
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
  return UnwrapObject(jniEnv, val, sHiddenKey, jVal);
}

int Conv::UnwrapObject(JNIEnv *jniEnv, Handle<Object> val, Handle<String> key, jobject *jVal) {
  int result = ErrorNotfound;
  Local<Value> hiddenVal = val->GetHiddenValue(key);
  if(!hiddenVal.IsEmpty() && !hiddenVal->IsUndefined()) {
    jobject extRef = (jobject)External::Unwrap(hiddenVal);
    if(jniEnv->GetObjectRefType(extRef) == JNIWeakGlobalRefType) {
      jobject localRef = jniEnv->NewLocalRef(extRef);
      if(localRef == 0) {
        /* the Java object died */
        jniEnv->DeleteGlobalRef(extRef);
        val->DeleteHiddenValue(key);
      } else {
        /* the java object is alive */
        *jVal = localRef;
        result = OK;
      }
    } else {
      /* the object is strongly referenced */
      *jVal = extRef;
      result = OK;
    }
  }
  return result;
}

int Conv::WrapV8Object(JNIEnv *jniEnv, Handle<Function> val, jobject *jVal) {
  Persistent<Function> pVal = Persistent<Function>::New(val);
  jobject ob = jniEnv->NewObject(jni.anode.js.JSFunction.class_, jni.anode.js.JSFunction.ctor, asLong(pVal));
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
  int result = interface->UserCreate(jniEnv, asLong(pVal), &ob);
  if(result == OK) {
    result = BindToV8Object(jniEnv, val, interface->getHiddenKey(), ob, jVal);
  }
  return result;
}

int Conv::BindToV8Object(JNIEnv *jniEnv, Handle<Object> val, Handle<String> key, jobject jLocal, jobject *jHard) {
  if(!jLocal || jniEnv->ExceptionCheck()) {
    jniEnv->ExceptionClear();
    return ErrorVM;
  }
  jobject jWeak = jniEnv->NewWeakGlobalRef(jLocal);
  val->SetHiddenValue(key, External::Wrap(jWeak));
  *jHard = jLocal;
  return OK;
}

int Conv::ToJavaObject(JNIEnv *jniEnv, Handle<Value> val, int expectedType, jobject *jVal) {

  /* empty, null and undefined convert to null for any nullable type */
  if((expectedType >= TYPE__OB_START) && (val.IsEmpty() || val->IsNull() || val->IsUndefined())) {
    *jVal = 0;
    return OK;
  }

  if(isSequence(expectedType))
    return ToJavaSequence(jniEnv, val, getComponentType(expectedType), (jarray *)jVal);
  
  if(isArray(expectedType))
    return arrayConv->ToJavaArray(jniEnv, val, getComponentType(expectedType), jVal);
  
  if(isMap(expectedType))
    return ToJavaMap(jniEnv, val, getComponentType(expectedType), jVal);
  
  if(isInterface(expectedType))
    return ToJavaInterface(jniEnv, val, getClassId(expectedType), jVal);
  
  if(isDict(expectedType))
    return ToJavaDict(jniEnv, val, getClassId(expectedType), jVal);
  
  HandleScope scope;
  jobject ob;
  switch(expectedType) {
    default:
      return ErrorType;
    case TYPE_BOOL: {
      bool isVoid = val.IsEmpty() ? false : val->BooleanValue();
      ob = jniEnv->CallStaticObjectMethod(jni.anode.js.JSValue_Bool.class_, jni.anode.js.JSValue_Bool.ctor, isVoid);
      break;
    }
    case TYPE_BYTE:
    case TYPE_INT: {
      jint intValue = val.IsEmpty() ? 0 : val->Int32Value();
      ob = jniEnv->CallStaticObjectMethod(jni.anode.js.JSValue_Long.class_, jni.anode.js.JSValue_Long.ctor, (jlong)intValue);
      break;
    }
    case TYPE_LONG: {
      jlong longValue = val.IsEmpty() ? 0 : val->IntegerValue();
      ob = jniEnv->CallStaticObjectMethod(jni.anode.js.JSValue_Long.class_, jni.anode.js.JSValue_Long.ctor, longValue);
      break;
    }
    case TYPE_DOUBLE: {
      jdouble doubleValue = val.IsEmpty() ? 0 : val->NumberValue();
      ob = jniEnv->CallStaticObjectMethod(jni.anode.js.JSValue_Double.class_, jni.anode.js.JSValue_Double.ctor, doubleValue);
      break;
    }
    case TYPE_STRING:
      return ToJavaString(jniEnv, val, (jstring *)jVal);
    case TYPE_DATE:
      return ToJavaDate(jniEnv, val, jVal);
    case TYPE_OBJECT:
      return ToNaturalJavaObject(jniEnv, val, jVal);
    case TYPE_OBJECT|TYPE_BOOL: {
      bool isVoid = val->BooleanValue();
      ob = jniEnv->NewObject(jni.java.lang.Boolean.class_, jni.java.lang.Boolean.ctor, isVoid);
      break;
    }
    case TYPE_OBJECT|TYPE_BYTE: {
      jbyte byteValue = val->Int32Value();
      ob = jniEnv->NewObject(jni.java.lang.Byte.class_, jni.java.lang.Byte.ctor, byteValue);
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

int Conv::ToJavaSequence(JNIEnv *jniEnv, Handle<Value> val, int componentType, jarray *jVal) {
  Local<Object> oVal;
  Local<Value> vLength;
  if(val.IsEmpty() || val->IsNull() || val->IsUndefined()) {
    *jVal = 0;
    return OK;
  }
  if(!val->IsObject())
    return ErrorType;
  
  oVal = val->ToObject();
  vLength = oVal->Get(sLength);
  if(vLength.IsEmpty() || !vLength->IsNumber())
    return ErrorType;
  
  jarray ob;
  jint len = vLength->Int32Value();
  if(isJavaObject(componentType)) {
    jclass componentClass;
    if(isInterfaceOrDict(componentType)) {
      Interface *interface = env->getInterface(getClassId(componentType));
      componentClass = interface->getDeclaredClass();
    } else {
      switch(componentType) {
        default:
          return ErrorType;
        case TYPE_STRING:
          componentClass = jni.java.lang.String.class_;
          break;
        case TYPE_DATE:
          componentClass = jni.java.util.Date.class_;
          break;
        case TYPE_OBJECT:
          componentClass = jni.anode.js.JSObject.class_;
          break;
      }
    }
    ob = jniEnv->NewObjectArray(len, componentClass, 0);
    if(ob) {
      int res = OK;
      for(int i = 0; i < len; i++) {
        jobject item;
        res = ToJavaObject(jniEnv, oVal->Get(i), componentType, &item);
        if(res != OK) break;
        jniEnv->SetObjectArrayElement((jobjectArray)ob, i, item);
      }
    }
  } else {
    switch(componentType) {
      default:
        return ErrorType;
      case TYPE_BYTE:
        ob = jniEnv->NewByteArray(len);
        if(ob) {
          /* FIXME: see if this case can be optimised */
          jbyte *buf = jniEnv->GetByteArrayElements((jbyteArray)ob, 0);
          for(int i = 0; i < len; i++)
            buf[i] = (jbyte)oVal->Get(i)->Uint32Value();
          
          jniEnv->ReleaseByteArrayElements((jbyteArray)ob, buf, 0);
          break;
        }
      case TYPE_INT:
        ob = jniEnv->NewIntArray(len);
        if(ob) {
          jint *buf = jniEnv->GetIntArrayElements((jintArray)ob, 0);
          for(int i = 0; i < len; i++)
            buf[i] = oVal->Get(i)->Int32Value();
          
          jniEnv->ReleaseIntArrayElements((jintArray)ob, buf, 0);
          break;
        }
      case TYPE_LONG:
        ob = jniEnv->NewLongArray(len);
        if(ob) {
          jlong *buf = jniEnv->GetLongArrayElements((jlongArray)ob, 0);
          for(int i = 0; i < len; i++)
            buf[i] = oVal->Get(i)->IntegerValue();
          
          jniEnv->ReleaseLongArrayElements((jlongArray)ob, buf, 0);
          break;
        }
      case TYPE_DOUBLE:
        ob = jniEnv->NewDoubleArray(len);
        if(ob) {
          jdouble *buf = jniEnv->GetDoubleArrayElements((jdoubleArray)ob, 0);
          if(!buf) return ErrorMem;
          for(int i = 0; i < len; i++)
            buf[i] = oVal->Get(i)->NumberValue();
          
          jniEnv->ReleaseDoubleArrayElements((jdoubleArray)ob, buf, 0);
          break;
        }
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

int Conv::ToJavaMap(JNIEnv *jniEnv, Handle<Value> val, int componentType, jobject *jVal) {
  Local<Object> oVal;
  Local<Array> aPropertyNames;
  if(val.IsEmpty() || val->IsNull() || val->IsUndefined()) {
    *jVal = 0;
    return OK;
  }
  if(!val->IsObject())
    return ErrorType;
  
  oVal = val->ToObject();
  aPropertyNames = oVal->GetOwnPropertyNames();
  int len = aPropertyNames->Length();

  jobject ob = jniEnv->NewObject(mapClass, mapCtor);
  if(ob) {
    int res = OK;
    for(int i = 0; i < len; i++) {
      Local<String> key = Local<String>::Cast(aPropertyNames->Get(i));
      jstring jKey; jobject item;
      res = ToJavaString(jniEnv, key, &jKey);
      if(res != OK) break;
      res = ToJavaObject(jniEnv, oVal->Get(key), componentType, &item);
      if(res != OK) break;
      jniEnv->CallObjectMethod(ob, mapPut, jKey, item);
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

int Conv::ToJavaInterface(JNIEnv *jniEnv, Handle<Value> val, classId clsid, jobject *jVal) {
  if(val.IsEmpty() || !val->IsObject())
    return ErrorType;
  Handle<Object> oVal = Handle<Object>::Cast(val);
  int result = UnwrapInterface(jniEnv, oVal, clsid, jVal);
  if(result == ErrorNotfound)
    result = WrapV8Interface(jniEnv, oVal, clsid, jVal);
  return result;
}

int Conv::ToJavaDict(JNIEnv *jniEnv, Handle<Value> val, classId clsid, jobject *jVal) {
  if(!val->IsObject()) return ErrorType;
  Interface *interface = env->getInterface(clsid);
  return interface ? interface->DictCreate(jniEnv, Handle<Object>::Cast(val), jVal) : ErrorInvalid;
}

int Conv::ToJavaDate(JNIEnv *jniEnv, Handle<Value> val, jobject *jVal) {
  double dateTime;
  if(val->IsDate()) {
    dateTime = Date::Cast(*val)->NumberValue();
  } else {
    dateTime = val->NumberValue();
  }
  jobject ob = jniEnv->NewObject(jni.java.util.Date.class_, jni.java.util.Date.ctor, (jlong)dateTime);
  if(ob) {
    *jVal = ob;
    return OK;
  }
  if(jniEnv->ExceptionCheck())
    jniEnv->ExceptionClear();
  return ErrorVM;
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
  Handle<String> vString;
  Handle<Value> empty, vRes;
  jstring ob;
  char buf[64];
  switch(GetNaturalType(val)) {
    default: {
      if(val->IsObject()) {
        /* call ToString() in javascript */
        Handle<Object> oVal = val->ToObject();
        Handle<Value> vToString = oVal->Get(sToString);
        if(!vToString.IsEmpty() && vToString->IsFunction()) {
          Handle<Function> fToString = Handle<Function>::Cast(vToString);
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

/****************************
 * Java to V8 conversions
 ****************************/

int Conv::ToV8Value(JNIEnv *jniEnv, jobject jVal, int expectedType, Handle<Value> *val) {
  if(jVal == 0) {
    *val = Undefined();
    return OK;
  }
  
  HandleScope scope;
  int result = OK;
  if(expectedType == TYPE_NONE || expectedType == TYPE_OBJECT) {
    expectedType = ob2Type(jniEnv, jVal);
    if(expectedType == TYPE_INVALID)
      return ErrorType;
  }

  if(isBase(expectedType & ~TYPE_OBJECT)) {
    Handle<Value> baseVal;
    result = ToV8Base(jniEnv, jVal, expectedType, &baseVal);
    if(result == OK)
      *val = scope.Close(Handle<Value>(baseVal));
    return result;
  }
  
  if(isSequence(expectedType)) {
    Handle<Array> seqVal;
    result = ToV8Sequence(jniEnv, (jarray)jVal, getComponentType(expectedType), &seqVal);
    if(result == OK)
      *val = scope.Close(Handle<Value>(seqVal));
    return result;
  }
  
  if(isArray(expectedType)) {
    Handle<Object> arrayVal;
    result = arrayConv->ToV8Array(jniEnv, jVal, getComponentType(expectedType), &arrayVal);
    if(result == OK)
      *val = scope.Close(Handle<Value>(arrayVal));
    return result;
  }

  if(isMap(expectedType)) {
    Handle<Object> mapVal;
    result = ToV8Map(jniEnv, jVal, getComponentType(expectedType), &mapVal);
    if(result == OK)
      *val = scope.Close(Handle<Value>(mapVal));
    return result;
  }
  
  if(isInterface(expectedType)) {
    Handle<Object> objectVal;
    result = ToV8Interface(jniEnv, jVal, getClassId(expectedType), &objectVal);
    if(result == OK)
      *val = scope.Close(Handle<Value>(objectVal));
    return result;
  }
  
  if(isDict(expectedType)) {
    Handle<Object> objectVal;
    result = ToV8Dict(jniEnv, jVal, getClassId(expectedType), &objectVal);
    if(result == OK)
      *val = scope.Close(Handle<Value>(objectVal));
    return result;
  }
  return ErrorInternal;
}
  
int Conv::ToV8Base(JNIEnv *jniEnv, jobject jVal, int expectedType, Handle<Value> *val) {
  HandleScope scope;
  Handle<Value> local;
  int result = OK;
  switch(expectedType) {
    default:
      return ErrorType;
    case TYPE_BOOL: {
      bool booleanValue = jniEnv->CallBooleanMethod(jVal, jni.anode.js.JSValue_Bool.getter);
      local = Handle<Value>(Boolean::New(booleanValue));
      break;
    }
    case TYPE_BYTE: {
      jbyte byteValue = (jbyte)jniEnv->CallLongMethod(jVal, jni.anode.js.JSValue_Long.getter);
      local = Handle<Value>(Number::New(byteValue));
      break;
    }
    case TYPE_INT: {
      jint intValue = (jint)jniEnv->CallLongMethod(jVal, jni.anode.js.JSValue_Long.getter);
      local = Handle<Value>(Number::New(intValue));
      break;
    }
    case TYPE_LONG: {
      jlong longValue = jniEnv->CallLongMethod(jVal, jni.anode.js.JSValue_Long.getter);
      local = Handle<Value>(Number::New(longValue));
      break;
    }
    case TYPE_DOUBLE: {
      jdouble doubleValue = jniEnv->CallDoubleMethod(jVal, jni.anode.js.JSValue_Double.getter);
      local = Handle<Value>(Number::New(doubleValue));
      break;
    }
    case TYPE_STRING: {
      Handle<String> stringVal;
      result = ToV8String(jniEnv, (jstring)jVal, &stringVal);
      if(result == OK)
        local = Handle<Value>(stringVal);
      break;
    }
    case TYPE_DATE: {
      result = ToV8Date(jniEnv, jVal, &local);
      break;
    }
    case TYPE_OBJECT|TYPE_BOOL: {
      bool booleanValue = jniEnv->CallBooleanMethod(jVal, jni.java.lang.Boolean.getter);
      local = Handle<Value>(Boolean::New(booleanValue));
      break;
    }
    case TYPE_OBJECT|TYPE_BYTE: {
      jbyte byteValue = jniEnv->CallByteMethod(jVal, jni.java.lang.Byte.getter);
      local = Handle<Value>(Number::New(byteValue));
      break;
    }
    case TYPE_OBJECT|TYPE_INT: {
      jint intValue = jniEnv->CallIntMethod(jVal, jni.java.lang.Integer.getter);
      local = Handle<Value>(Number::New(intValue));
      break;
    }
    case TYPE_OBJECT|TYPE_LONG: {
      jlong longValue = jniEnv->CallLongMethod(jVal, jni.java.lang.Long.getter);
      local = Handle<Value>(Number::New(longValue));
      break; }
    case TYPE_OBJECT|TYPE_DOUBLE: {
      double doubleValue = jniEnv->CallDoubleMethod(jVal, jni.java.lang.Double.getter);
      local = Handle<Value>(Number::New(doubleValue));
      break;
    }
  }
  if(jniEnv->ExceptionCheck()) {
    jniEnv->ExceptionClear();
    result = ErrorVM;
  }
  *val = scope.Close(local);
  return result;
}

int Conv::ToV8Sequence(JNIEnv *jniEnv, jarray jVal, int expectedType, Handle<Array> *val) {
  unsigned int componentType = getComponentType(expectedType);
  int length = jniEnv->GetArrayLength(jVal);
  Local<Array> lVal = Local<Array>(Array::New(length));
  int result = OK;
  if(isJavaObject(componentType)) {
    jobjectArray jOVal = (jobjectArray)jVal;
    for(int i = 0; i < length; i++) {
      Local<Value> elt;
      jobject jElt = jniEnv->GetObjectArrayElement(jOVal, i);
      result = ToV8Value(jniEnv, jElt, componentType, &elt);
      if(result != OK) break;
      lVal->Set(i, elt);
    }
    if(result == OK) {
      *val = lVal;
    }
    if(jniEnv->ExceptionCheck()) {
      jniEnv->ExceptionClear();
      result = ErrorVM;
    }
    return result;
  }
  /* FIXME: use optimised buffers */
  switch(componentType) {
    case TYPE_BYTE: {
      jbyteArray jBVal = (jbyteArray)jVal;
      jbyte *elts = jniEnv->GetByteArrayElements(jBVal, 0);
      for(int i = 0; i < length; i++) {
        lVal->Set(i, Number::New(elts[i]));
      }
      jniEnv->ReleaseByteArrayElements(jBVal, elts, 0);
    }
      break;
    case TYPE_INT: {
      jintArray jIVal = (jintArray)jVal;
      jint *elts = jniEnv->GetIntArrayElements(jIVal, 0);
      for(int i = 0; i < length; i++) {
        lVal->Set(i, Number::New(elts[i]));
      }
      jniEnv->ReleaseIntArrayElements(jIVal, elts, 0);
    }
      break;
    case TYPE_LONG: {
      jlongArray jJVal = (jlongArray)jVal;
      jlong *elts = jniEnv->GetLongArrayElements(jJVal, 0);
      for(int i = 0; i < length; i++) {
        lVal->Set(i, Number::New(elts[i]));
      }
      jniEnv->ReleaseLongArrayElements(jJVal, elts, 0);
    }
      break;
    case TYPE_DOUBLE: {
      jdoubleArray jDVal = (jdoubleArray)jVal;
      jdouble *elts = jniEnv->GetDoubleArrayElements(jDVal, 0);
      for(int i = 0; i < length; i++) {
        lVal->Set(i, Number::New(elts[i]));
      }
      jniEnv->ReleaseDoubleArrayElements(jDVal, elts, 0);
    }
      break;
    default:
      result = ErrorType;
  }
  if(result == OK) {
    *val = lVal;
  }
  if(jniEnv->ExceptionCheck()) {
    jniEnv->ExceptionClear();
    result = ErrorVM;
  }
  return result;
}

int Conv::ToV8Map(JNIEnv *jniEnv, jobject jVal, int expectedType, Handle<Object> *val) {
  unsigned int componentType = getComponentType(expectedType);
  Local<Object> lVal = Local<Object>(Object::New());
  int result = OK;
  if(jniEnv->CallIntMethod(jVal, mapSize) > 0) {
    jobjectArray jKeys = (jobjectArray)jniEnv->CallObjectMethod(jniEnv->CallObjectMethod(jVal, mapKeySet), setToArray);
    int length = jniEnv->GetArrayLength(jKeys);
    for(int i = 0; i < length; i++) {
      jstring jKey; jobject jElt;
      Local<String> key; Local<Value> elt;
      jKey = (jstring)jniEnv->GetObjectArrayElement(jKeys, i);
      jElt = jniEnv->CallObjectMethod(jVal, mapGet, jKey);
      result = ToV8Value(jniEnv, jElt, componentType, &elt);
      if(result != OK) break;
      result = ToV8String(jniEnv, jKey, &key);
      if(result != OK) break;
      lVal->Set(key, elt);
    }
  }
  if(result == OK) {
    *val = lVal;
  }
  if(jniEnv->ExceptionCheck()) {
    jniEnv->ExceptionClear();
    result = ErrorVM;
  }
  return result;
}

int Conv::ToV8Interface(JNIEnv *jniEnv, jobject jVal, classId clsid, Handle<Object> *val) {
LOGV("ToV8Interface 1\n");
  int result = UnwrapObject(jniEnv, jVal, val);
LOGV("ToV8Interface 2\n");
  if(result == ErrorNotfound) {
LOGV("ToV8Interface 3\n");
    Handle<Object> vInst;
    Interface *interface = env->getInterface(clsid);
LOGV("ToV8Interface 4\n");
    result = interface->PlatformCreate(jniEnv, jVal, &vInst);
LOGV("ToV8Interface 5\n");
    if(result == OK) {
LOGV("ToV8Interface 6\n");
      result = BindToJavaObject(jniEnv, jVal, vInst, interface);
LOGV("ToV8Interface 7\n");
      if(result == OK)
        *val = vInst;
    }
  }
  return result;
}

int Conv::ToV8Dict(JNIEnv *jniEnv, jobject jVal, classId clsid, Handle<Object> *val) {
  Interface *interface = env->getInterface(clsid);
  Local<Object> lVal = Object::New();
  int result = interface->DictExport(jniEnv, jVal, lVal);
  if(result == OK)
    *val = lVal;
  return result;
}

int Conv::ToV8String(JNIEnv *jniEnv, jstring jVal, Handle<String> *val) {
  int len = jniEnv->GetStringUTFLength(jVal);
  const char *data = jniEnv->GetStringUTFChars(jVal, 0);
  *val = String::New(data, len);
  jniEnv->ReleaseStringUTFChars(jVal, data);
  return OK;
}

int Conv::ToV8Date(JNIEnv *jniEnv, jobject jVal, Handle<Value> *val) {
  jlong dateVal = jniEnv->CallLongMethod(jVal, jni.java.util.Date.getter);
  *val = Date::New((double)dateVal);
  return OK;
}

int Conv::UnwrapObject(JNIEnv *jniEnv, jobject jVal, Handle<Object> *val) {
  jlong pVal = jniEnv->GetLongField(jVal, instHandle);
  if(pVal) {
    Local<Object> lVal = Local<Object>(*asHandle(pVal));
    if(!lVal.IsEmpty()) {
      *val = lVal;
      return OK;
    }
  }
  return ErrorNotfound;
}

int Conv::BindToJavaObject(JNIEnv *jniEnv, jobject jLocal, Handle<Object> val, jobject *jGlobal) {
  int result = OK;
  Persistent<Object> pVal = Persistent<Object>::New(val);
  jobject ob = jniEnv->NewGlobalRef(jLocal);
  pVal.MakeWeak(ob, releaseJavaRef);
  val->SetPointerInInternalField(0, ob);
  jniEnv->SetLongField(jLocal, instHandle, asLong(pVal));
  *jGlobal = ob;
  return result;
}

int Conv::BindToJavaObject(JNIEnv *jniEnv, jobject jLocal, Handle<Object> val, Handle<String> key) {
  jobject ob;
  int result = BindToJavaObject(jniEnv, jLocal, val, &ob);
  if(result == OK)
    val->SetHiddenValue(key, External::Wrap(ob));
  return result;
}

int Conv::BindToJavaObject(JNIEnv *jniEnv, jobject jLocal, Handle<Object> val, Interface *interface) {
  jobject ob;
  int result = BindToJavaObject(jniEnv, jLocal, val, &ob);
  if(result == OK) {
    Handle<Value> vOb = External::Wrap(ob);
    while(interface != 0) {
      val->SetHiddenValue(interface->getHiddenKey(), vOb);
      interface = interface->getParent();
    }
  }
  return result;
}

/* called from v8 when Weak Persistent references
 * to Java objects are eligible for collection */
void Conv::releaseJavaRef(Persistent<Value> instHandle, void *jGlobalRef) {
  jobject ob = (jobject)jGlobalRef;
  Env *env = Env::getEnv_nocheck();
  JNIEnv *jniEnv = Env::getEnv_nocheck()->getVM()->getJNIEnv();
  jniEnv->SetLongField(ob, env->getConv()->instHandle, 0);
  jniEnv->DeleteGlobalRef(ob);
  instHandle.Dispose();
}

/* called by the Java environment for objects that have been finalized */
void Conv::releaseV8Handle(JNIEnv *jniEnv, Persistent<Object> val, int type) {
  HandleScope scope;
  Handle<String> sHiddenKey;
  Interface *interface = 0;
  ArrayType *arr = 0;
  if(type == -1) {
    sHiddenKey = sObjectHiddenKey;
  } else if(isArray(type)) {
    arrayConv->GetRefsForComponentType(jniEnv, getComponentType(type), &arr);
    sHiddenKey = arr->getHiddenKey();
  } else if(isInterface(type)) {
    interface = env->getInterface(getClassId(type));
    sHiddenKey = interface->getHiddenKey();
  }
  Local<Value> hiddenVal = val->GetHiddenValue(sHiddenKey);
  if(!hiddenVal.IsEmpty() && !hiddenVal->IsUndefined()) {
    jobject extRef = (jobject)External::Unwrap(hiddenVal);
    jniEnv->DeleteGlobalRef(extRef);
    val->DeleteHiddenValue(sHiddenKey);
    if(interface) {
      while((interface = interface->getParent())) {
        val->DeleteHiddenValue(interface->getHiddenKey());
      }
    }
  }
  val.Dispose();
}

Handle<String> Conv::getTypeKey(unsigned int type) {
  char keyStr[32];
  sprintf(keyStr, "node::type::%x", type);
  return Local<String>::New(String::New(keyStr));
}

jstring Conv::getJavaClassName(JNIEnv *jniEnv, jclass class_, bool replace) {
  jstring name = (jstring)jniEnv->CallObjectMethod(class_, classGetName);
  if(replace)
    name = (jstring)jniEnv->CallObjectMethod(name, stringReplace, '.', '/');
  return name;
}

Handle<String> Conv::getV8ClassName(JNIEnv *jniEnv, jclass class_) {
  Local<String> name;
  ToV8String(jniEnv, getJavaClassName(jniEnv, class_, false), &name);
  return name;
}

void Conv::ThrowV8ExceptionForThrowable(JNIEnv *jniEnv, jthrowable throwable) {
  int result;
  Handle<Value> val;
  if(jniEnv->IsInstanceOf(throwable, baseClass)) {
    result = ToV8Value(jniEnv, throwable, TYPE_OBJECT, &val);
  } else {
    Handle<String> excString;
    jstring jExcString = (jstring)jniEnv->CallObjectMethod(throwable, objectToString);
    result = ToV8String(jniEnv, jExcString, &excString);
    if(result == OK)
      val = Local<Value>(*excString);
  }
  if(result != OK)
    val = Exception::Error(String::New("bridge: Unknown error (nested error)"));
  
  ThrowException(val);
}


void Conv::ThrowV8ExceptionForErrno(int errno) {
  switch(errno) {
    case OK:
      break;
    case ErrorType:
      ThrowException(Exception::TypeError(String::New("TypeError")));
      break;
    case ErrorNotfound:
      ThrowException(Exception::ReferenceError(String::New("ReferenceError")));
      break;
    case ErrorVM:
      ThrowException(Exception::Error(String::New("bridge: Internal VM error")));
      break;
    default:
      char buf[32];
      sprintf(buf, "bridge: Unknown error: %d", errno);
      ThrowException(Exception::Error(String::New(buf)));
  }
}
  
bool Conv::CheckForException(JNIEnv *jniEnv) {
  if(jniEnv->ExceptionCheck()) {
    jthrowable jExc = jniEnv->ExceptionOccurred();
    jniEnv->ExceptionClear();
    ThrowV8ExceptionForThrowable(jniEnv, jExc);
    return true;
  }
  return false;
}
