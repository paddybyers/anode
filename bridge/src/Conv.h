#ifndef BRIDGE_CONV_H
#define BRIDGE_CONV_H

#include "defines.h"

namespace bridge {

class ArrayConv;
class Env;
class Interface;

/* Class to perform conversions between v8 and java values
 * and hold assocoiated per-env state */

typedef struct classRefs_s {
  jclass class_;
  jclass primitive;
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
  
  inline ArrayConv *getArrayConv() {return arrayConv;}

  /* type conversions */
  jclass type2Class(int type);
  int class2Type(JNIEnv *jniEnv, jclass class_, jclass *componentType = 0);
  int ob2Type(JNIEnv *jniEnv, jobject ob, jclass *componentType = 0);
  
  /* Returns a type value representing the intrinsic
   * v8 type information, without reference to an
   * expected type */
  int GetNaturalType(v8::Handle<v8::Value> val);
  int GetNaturalType(v8::Handle<v8::Object> val);
  
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
  int ToNaturalJavaObject(JNIEnv *jniEnv, v8::Handle<v8::Value> val, jobject *jVal);

  /* attempts to unwrap a previously wrapped object,
   * when there is a known expected interface type */
  int UnwrapInterface(JNIEnv *jniEnv, v8::Handle<v8::Object> val, classId class_, jobject *jVal);

  /* attempts to unwrap a previously wrapped object */
  int UnwrapObject(JNIEnv *jniEnv, v8::Handle<v8::Object> val, v8::Handle<v8::String> key, jobject *jVal);
  
  /* wraps a V8 Object as a Java JSObject|JSArray|JSFunction according to type */
  int WrapV8Object(JNIEnv *jniEnv, v8::Handle<v8::Function> val, jobject *jVal);
  int WrapV8Object(JNIEnv *jniEnv, v8::Handle<v8::Object> val, jobject *jVal);
  
  /* wraps a V8 object that implements an interface */
  int WrapV8Interface(JNIEnv *jniEnv, v8::Handle<v8::Object> val, classId class_, jobject *jVal);
  
  /* connect the handles */
  int BindToV8Object(JNIEnv *jniEnv, v8::Handle<v8::Object> val, v8::Handle<v8::String> key, jobject jLocal, jobject *jGlobal);

  /* Converts to a java object based on the expected type
   * If the expected type is not specified, then the conversion
   * is performed based on the natural type of the given JS argument.
   * JS Native Function, Object and Array
   * types are wrapped; if previously wrapped then the
   * original wrapper value is returned.
   * Wrapped exported objects are unwrapped.
   * A LocalRef is returned */
  int ToJavaObject(JNIEnv *jniEnv, v8::Handle<v8::Value> val, int expectedType, jobject *jVal);
  int ToJavaSequence(JNIEnv *jniEnv, v8::Handle<v8::Value> val, int componentType, jarray *jVal);
  int ToJavaInterface(JNIEnv *jniEnv, v8::Handle<v8::Value> val, classId clsid, jobject *jVal);
  int ToJavaDict(JNIEnv *jniEnv, v8::Handle<v8::Value> val, classId clsid, jobject *jVal);
  int ToJavaDate(JNIEnv *jniEnv, v8::Handle<v8::Value> val, jobject *jVal);
  
  /* converts to a java string */
  int ToJavaString(JNIEnv *jniEnv, v8::Handle<v8::Value> val, jstring *jVal);
  int ToJavaString(JNIEnv *jniEnv, const char *valUtf, jstring *jVal);
  static int ToJavaString(JNIEnv *jniEnv, v8::Handle<v8::String> val, jstring *jVal);

  /****************************
   * Java to V8 conversions
   ****************************/

  int ToV8Value(JNIEnv *jniEnv, jobject jVal, int expectedType, v8::Handle<v8::Value> *val);
  int ToV8Base(JNIEnv *jniEnv, jobject jVal, int expectedType, v8::Handle<v8::Value> *val);
  int ToV8Sequence(JNIEnv *jniEnv, jarray jVal, int expectedType, v8::Handle<v8::Array> *val);
  int ToV8Interface(JNIEnv *jniEnv, jobject jVal, classId clsid, v8::Handle<v8::Object> *val);
  int ToV8Dict(JNIEnv *jniEnv, jobject jVal, classId clsid, v8::Handle<v8::Object> *val);
  int ToV8String(JNIEnv *jniEnv, jstring jVal, v8::Handle<v8::String> *val);
  int ToV8Date(JNIEnv *jniEnv, jobject jVal, v8::Handle<v8::Value> *val);
  
  /* attempts to unwrap a previously wrapped object */
  int UnwrapObject(JNIEnv *jniEnv, jobject jVal, v8::Handle<v8::Object> *val);

  /* connect the handles */
  int BindToJavaObject(JNIEnv *jniEnv, jobject jLocal, v8::Handle<v8::Object> val, jobject *jGlobal);
  int BindToJavaObject(JNIEnv *jniEnv, jobject jLocal, v8::Handle<v8::Object> val, v8::Handle<v8::String> key);
  int BindToJavaObject(JNIEnv *jniEnv, jobject jLocal, v8::Handle<v8::Object> val, Interface *interface);
  
  void releaseV8Handle(JNIEnv *jniEnv, v8::Persistent<v8::Object> intHandle, int type);
  static void releaseJavaRef(v8::Persistent<v8::Value> instHandle, void *jGlobalRef);
  static v8::Handle<v8::String> getTypeKey(unsigned int type);
  jstring getJavaClassName(JNIEnv *jniEnv, jclass class_, bool replace);
  v8::Handle<v8::String> getV8ClassName(JNIEnv *jniEnv, jclass class_);
  
  static void ThrowV8ExceptionForErrno(int errno);
  
private:
  Env *env;
  ArrayConv *arrayConv;
  v8::Persistent<v8::String> sObjectHiddenKey;
  v8::Persistent<v8::String> sToString;
  v8::Persistent<v8::String> sLength;
  refs_t jni;
  classRefs *typeToRef[TYPE___END];
  jclass classClass;
  jclass baseClass;
  jclass dictClass;
  jmethodID classIsArray;
  jmethodID classIsAssignableFrom;
  jmethodID classIsPrimitive;
  jmethodID classGetComponentType;
  jmethodID classGetName;
  jmethodID stringReplace;
  jfieldID instHandle;
  jfieldID instType;
};

} // namespace bridge
#endif
