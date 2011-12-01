#ifndef BRIDGE_VM_H
#define BRIDGE_VM_H

#include "defines.h"

class VM {
public:
	VM();
	virtual    ~VM();
	JNIEnv     *getJNIEnv();
  virtual int createContext(jobject jEnv, jobject jExports, jobject *jCtx) = 0;

protected:
	JNIEnv     *jniEnv;
  /* module lifecycle */
  jclass      jContextClass;
  jmethodID   createContextMethodId;
};

#endif
