#include "VM.h"

VM::VM() : jniEnv(0) {}

VM::~VM() {}

JNIEnv *VM::getJNIEnv() {
	return jniEnv;
}
