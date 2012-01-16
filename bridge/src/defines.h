#ifndef BRIDGE_DEFINES_H
#define BRIDGE_DEFINES_H

/****************
 * misc defines
 ****************/

#define LIB_EXPORT __attribute__ ((visibility("default")))

/******************
 * global inclusion
 ******************/

#include <v8.h>

#ifdef __APPLE__
# include <JavaVM/jni.h>
#else
# include <jni.h>
#endif

/****************
 * log
 ****************/
#define DEBUG
#ifdef DEBUG
# ifdef ANDROID
#  include <android/log.h>
#  define DEBUG_TAG "bridge-core"
#  define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, DEBUG_TAG, __VA_ARGS__)
# else
#  define LOGV(...) fprintf(stderr, __VA_ARGS__)
# endif
#else
# define LOGV(...)
#endif
/****************
 * error codes
 ****************/

enum {
	OK              =  0,
	ErrorMem        = -1, /* out of memory */
	ErrorConfig     = -2, /* configuration error */
	ErrorVM         = -3, /* Java exception java framework code */
	ErrorIO         = -4, /* IO error */
	ErrorNotfound   = -5, /* resource not found */
	ErrorInvalid    = -6, /* invalid argument or value */
	ErrorType       = -7, /* incompatible type */
  ErrorJS         = -8, /* V8 exception in user code */
  ErrorInvocation = -9, /* Java exception in user (module) code */
  ErrorInternal   = -10, /* internal error */
  ErrorTBD        = -11
};

/****************
 * types
 ****************/

enum {
  TYPE_INVALID   = -1,
  TYPE_NONE      =  0,
  TYPE_UNDEFINED ,
  TYPE_NULL      ,
  TYPE_BOOL      ,
  TYPE_BYTE      ,
  TYPE_SHORT     , /* = 5 */
  TYPE_INT       ,
  TYPE_LONG      ,
  TYPE_DOUBLE    ,
  TYPE_STRING    ,
  TYPE__OB_START = TYPE_STRING,
  TYPE_FUNCTION  , /*= 10 */
  TYPE_DATE      ,
  TYPE_OBJECT    = 16,
  TYPE___END     ,
  TYPE_SEQUENCE  = 32,
  TYPE_ARRAY     = 64,
  TYPE_IDL       = 128,
  TYPE_MAP       = 256
};

typedef unsigned short classId;

inline classId getClassId(int type) {
  return (short)(type >> 16);
}

inline unsigned int getInterfaceType(classId clsid) {
  return (clsid << 16) | TYPE_IDL;
}

inline bool isDictClass(classId class_) { return class_ & 1; }

inline bool isInterfaceOrDict(int type) {
  return (type & TYPE_IDL) != 0;
}

inline bool isInterface(int type) {
  return isInterfaceOrDict(type) && !isDictClass(getClassId(type));
}

inline bool isDict(int type) {
  return isInterfaceOrDict(type) && isDictClass(getClassId(type));
}

inline bool isSequence(int type) {
  return (type & TYPE_SEQUENCE) != 0;
}

inline bool isArray(int type) {
  return (type & TYPE_ARRAY) != 0;
}

inline bool isBase(int type) {
  return (type < TYPE___END);
}

inline bool isJavaObject(int type) {
  return (type >= TYPE__OB_START);
}

inline int getComponentType(int type) {
  return (type & (~TYPE_ARRAY & ~TYPE_SEQUENCE));
}

inline int getArrayType(int type) {
  return (type | TYPE_ARRAY);
}

inline int getSequenceType(int type) {
  return (type | TYPE_SEQUENCE);
}

/******************************
 * stub modes
 ******************************/

enum MODE {
  MODE_IMPORT,
  MODE_EXPORT,
  MODE_VALUE
};

/******************************
 * object private field indices
 ******************************/

enum {
  PRIVATE_GLOBALREF,
  PRIVATE_UNUSED,
  PRIVATE_COUNT
};

/******************************
 * conv to jlong for Persistent<>
 ******************************/

inline jlong asLong(v8::Persistent<v8::Object> val) {
  return (jlong)*val;
}

inline v8::Persistent<v8::Object> asHandle(jlong jVal) {
  return v8::Persistent<v8::Object>((v8::Object*)jVal);
}

#endif
