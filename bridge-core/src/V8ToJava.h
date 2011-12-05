#ifndef BRIDGE_V8TOJAVA_H
#define BRIDGE_V8TOJAVA_H

#include "defines.h"

using namespace v8;

/* Class to perform conversions between v8 and java values
 * and hold assocoiated per-env state */

typedef struct {
  jclass class_;
  jmethodID ctor;
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
      classRefs JSValue_Bool;
      classRefs JSValue_Long;
      classRefs JSValue_Double;
    } js;
  } anode;
};

class V8ToJava {
public:
  V8ToJava(JNIEnv *jniEnv);
  void dispose(JNIEnv *jniEnv);
  ~V8ToJava();
  
  /* Returns a type value representing the intrinsic
   * v8 type information, without any input from
   * expected type */
  int GetType(Handle<Value> val);
  
  /* Converts to a java object that naturally represents
   * the intrinsic type, without any input from
   * expected type. JS Native Function, Object and Array
   * types are wrapped; if previously wrapped then the
   * original wrapper value is returned. Note that this
   * previous wrapper value may have previously been 
   * performed with the benefit of expected type information.
   * Wrapped exported objects are unwrapped.
   * A LocalRef is returned in jVal */
  int ToJavaObject(JNIEnv *jniEnv, Handle<Value> val, jobject *jVal);

  /* Converts to a java object based on the expected type
   * If the expected type is not specified, then the conversion
   * is performed based on the natural type of the given JS argument.
   * JS Native Function, Object and Array
   * types are wrapped; if previously wrapped then the
   * original wrapper value is returned. Note that this
   * previous wrapper value may have previously been 
   * performed with the benefit of expected type information.
   * Wrapped exported objects are unwrapped.
   * A LocalRef is returned */
  int ToJavaObject(JNIEnv *jniEnv, Handle<Value> val, int expectedType, jobject *jVal);
  int ToJavaObject(JNIEnv *jniEnv, Handle<Object> val, int expectedType, jobject *jVal);
  int ToJavaArray(JNIEnv *jniEnv, Handle<Object> val, int componentType, jarray *jVal);
  int ToJavaInterface(JNIEnv *jniEnv, Handle<Object> val, classId clsid, jobject *jVal);
  int ToJavaDate(JNIEnv *jniEnv, Handle<Object> val, jobject *jVal);
  
  /* converts to a java string */
  int ToJavaString(JNIEnv *jniEnv, Handle<Value> val, jstring *jVal);
  int ToJavaString(JNIEnv *jniEnv, const char *valUtf, jstring *jVal);
  static int ToJavaString(JNIEnv *jniEnv, Handle<String> val, jstring *jVal);
  
private:
  Persistent<String> sRefHiddenKey;
  Persistent<String> sToString;
  Persistent<String> sLength;
  refs_t jni;
};

#endif
