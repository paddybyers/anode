#include "VM.h"

VM::VM() : jniEnv(0) {}

VM::~VM() {
  jniEnv->DeleteGlobalRef(jContextClass);
}

JNIEnv *VM::getJNIEnv() {
	return jniEnv;
}
