#ifndef BRIDGE_CONV_H
#define BRIDGE_CONV_H

#include "defines.h"

class Env;

using namespace v8;

/* Class to perform conversions between v8 and java values
 * and hold assocoiated per-env state */

typedef struct {
  jclass class_;
  jmethodID ctor;
  jmethodID getter;
} classRefs;

struct refs_t {
  struct {
    struct {
      classRefs Boolean;
      classRefs Byte;
      classRefs Integer;
      classRefs Long;
      classRefs Double;
      classRefs String;
      classRefs Object;
    } lang;
    struct {
      classRefs Date;
    } util;
  } java;
  struct {
    struct {
      classRefs JSObject;
      classRefs JSArray;
      classRefs JSFunction;
      classRefs JSInterface;
      classRefs JSValue_Bool;
      classRefs JSValue_Long;
      classRefs JSValue_Double;
    } js;
  } anode;
};

class Conv {
public:
  Conv(Env *env, JNIEnv *jniEnv);
  void dispose(JNIEnv *jniEnv);
  ~Conv();

  /* type conversions */
  jclass type2Class(int type);
  int class2Type(JNIEnv *jniEnv, jclass class_, jclass *componentType = 0);
  int ob2Type(JNIEnv *jniEnv, jobject ob, jclass *componentType = 0);
  
  /* Returns a type value representing the intrinsic
   * v8 type information, without reference to an
   * expected type */
  int GetNaturalType(Handle<Value> val);
  int GetNaturalType(Handle<Object> val);
  
  /****************************
   * V8 to Java conversions
   ****************************/
  
  /* Converts to a java object that naturally represents
   * the intrinsic type, without reference to an
   * expected type. JS Native Function, Object and Array
   * types are wrapped; if previously wrapped then the
   * original wrapper value is returned.
   * Wrapped exported objects are unwrapped.
   * A LocalRef is returned in jVal */
  int ToNaturalJavaObject(JNIEnv *jniEnv, Handle<Value> val, jobject *jVal);

  /* attempts to unwrap a previously wrapped object,
   * when there is a known expected interface type */
  int UnwrapInterface(JNIEnv *jniEnv, Handle<Object> val, classId class_, jobject *jVal);

  /* attempts to unwrap a previously wrapped object */
  int UnwrapObject(JNIEnv *jniEnv, Handle<Object> val, Handle<String> key, jobject *jVal);
  
  /* wraps a V8 Object as a Java JSObject|JSArray|JSFunction according to type */
  int WrapV8Object(JNIEnv *jniEnv, Handle<Function> val, jobject *jVal);
  int WrapV8Object(JNIEnv *jniEnv, Handle<Array> val, jobject *jVal);
  int WrapV8Object(JNIEnv *jniEnv, Handle<Object> val, jobject *jVal);
  
  /* wraps a V8 object that implements an interface */
  int WrapV8Interface(JNIEnv *jniEnv, Handle<Object> val, classId class_, jobject *jVal);
  
  /* connect the handles */
  int BindToV8Object(JNIEnv *jniEnv, Handle<Object> val, Handle<String> key, jobject jLocal, jobject *jGlobal);

  /* Converts to a java object based on the expected type
   * If the expected type is not specified, then the conversion
   * is performed based on the natural type of the given JS argument.
   * JS Native Function, Object and Array
   * types are wrapped; if previously wrapped then the
   * original wrapper value is returned.
   * Wrapped exported objects are unwrapped.
   * A LocalRef is returned */
  int ToJavaObject(JNIEnv *jniEnv, Handle<Value> val, int expectedType, jobject *jVal);
  int ToJavaSequence(JNIEnv *jniEnv, Handle<Value> val, int componentType, jarray *jVal);
  int ToJavaArray(JNIEnv *jniEnv, Handle<Value> val, int componentType, jobject *jVal);
  int ToJavaInterface(JNIEnv *jniEnv, Handle<Value> val, classId clsid, jobject *jVal);
  int ToJavaDict(JNIEnv *jniEnv, Handle<Value> val, classId clsid, jobject *jVal);
  int ToJavaDate(JNIEnv *jniEnv, Handle<Value> val, jobject *jVal);
  
  /* converts to a java string */
  int ToJavaString(JNIEnv *jniEnv, Handle<Value> val, jstring *jVal);
  int ToJavaString(JNIEnv *jniEnv, const char *valUtf, jstring *jVal);
  static int ToJavaString(JNIEnv *jniEnv, Handle<String> val, jstring *jVal);

  /****************************
   * Java to V8 conversions
   ****************************/

  int ToV8Value(JNIEnv *jniEnv, jobject jVal, int expectedType, Handle<Value> *val);
  int ToV8Sequence(JNIEnv *jniEnv, jobject jVal, int expectedType, Handle<Array> *val);
  int ToV8Array(JNIEnv *jniEnv, jobject jVal, int expectedType, Handle<Object> *val);
  int ToV8Interface(JNIEnv *jniEnv, jobject jVal, int expectedType, Handle<Object> *val);
  int ToV8Dict(JNIEnv *jniEnv, jobject jVal, int expectedType, Handle<Object> *val);
  int ToV8String(JNIEnv *jniEnv, jstring jVal, Handle<String> *val);
  int ToV8Date(JNIEnv *jniEnv, jobject jVal, Handle<Date> *val);
  int ToV8Object(JNIEnv *jniEnv, jobject jVal, Handle<Object> *val);
  
  void releaseV8Handle(JNIEnv *jniEnv, Persistent<Object> intHandle, int classId);
  static void releaseJavaRef(Persistent<Value> instHandle, void *jGlobalRef);
  
private:
  Env *env;
  Persistent<String> sObjectHiddenKey;
  Persistent<String> sToString;
  Persistent<String> sLength;
  refs_t jni;
  classRefs *typeToRef[TYPE___END];
  jclass classClass;
  jmethodID classIsArray;
  jmethodID classGetComponentType;
  
};

#endif
