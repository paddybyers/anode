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
 * error codes
 ****************/

enum {
	OK            =  0,
	ErrorMem      = -1,
	ErrorConfig   = -2,
	ErrorVM       = -3,
	ErrorIO       = -4,
	ErrorNotfound = -5,
	ErrorInvalid  = -6,
	ErrorType     = -7,
  ErrorTBD      = -8
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
  TYPE_MAP       , /*= 10 */
  TYPE_FUNCTION  ,
  TYPE_DATE      ,
  TYPE_OBJECT    = 16,
  TYPE___END     ,
  TYPE_SEQUENCE  = 32,
  TYPE_ARRAY     = 64,
  TYPE_INTERFACE = 128
};

inline bool isInterface(int type) {
  return (type & TYPE_INTERFACE) != 0;
}

inline bool isSequence(int type) {
  return (type & TYPE_SEQUENCE) != 0;
}

inline bool isArray(int type) {
  return (type & TYPE_ARRAY) != 0;
}

inline int getComponentType(int type) {
  return (type & (~TYPE_ARRAY & ~TYPE_SEQUENCE));
}

typedef unsigned short classId;

inline classId getClassId(int type) {
  return (short)(type >> 16);
}

inline bool isValueType(classId class_) { return class_ & 1; }

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
