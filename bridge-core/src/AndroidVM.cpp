#include "AndroidVM.h"

AndroidVM::AndroidVM(JNIEnv *jniEnv, jobject androidContext) : VM() {
	this->jniEnv = jniEnv;
	this->androidContext = androidContext;
}

AndroidVM::~AndroidVM() {}

jobject AndroidVM::getAndroidContext() {
	return androidContext;
}
