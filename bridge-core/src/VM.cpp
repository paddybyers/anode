#include "VM.h"

using namespace bridge;

VM::VM() : jniEnv(0) {}

VM::~VM() {}

JNIEnv *VM::getJNIEnv() {
	return jniEnv;
}
