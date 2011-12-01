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
  TYPE_ARRAY     = 32,
  TYPE_INTERFACE = 64
};

inline bool isInterface(int type) {
  return (type & TYPE_INTERFACE) != 0;
}

inline bool isArray(int type) {
  return (type & TYPE_ARRAY) != 0;
}

inline int getComponentType(int type) {
  return (type & ~TYPE_ARRAY);
}

typedef unsigned short classId;

inline classId getClassId(int type) {
  return (short)(type >> 16);
}

/******************************
 * object private field indices
 ******************************/

enum {
  PRIVATE_GLOBALREF,
  PRIVATE_UNUSED,
  PRIVATE_COUNT
};

/******************************
 * union hack for Persistent<>
 ******************************/

inline jlong asLong2(v8::Persistent<v8::Object> val) {
  if(sizeof(val) == sizeof(jint))
    return (jlong)*((jint*)&val);
  if(sizeof(val) == sizeof(jlong))
    return *((jlong*)&val);
  return 0;
}

inline v8::Persistent<v8::Object> asHandle2(jlong jVal) {
  v8::Persistent<v8::Object> val = v8::Persistent<v8::Object>();
  if(sizeof(val) == sizeof(jint))
    *(jint *)&val = (jint)jVal;
  else if(sizeof(val) == sizeof(jlong))
    *(jlong *)&val = jVal;
  return val;
}

inline jlong asLong(v8::Persistent<v8::Object> val) {
  return (jlong)*val;
}

inline v8::Persistent<v8::Object> asHandle(jlong jVal) {
  return v8::Persistent<v8::Object>((v8::Object*)jVal);
}

#endif
