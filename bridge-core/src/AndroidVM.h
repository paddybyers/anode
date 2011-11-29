#ifndef BRIDGE_ANDROIDVM_H
#define BRIDGE_ANDROIDVM_H

#include "VM.h"

class AndroidVM : public VM {
public:
	AndroidVM(JNIEnv *jniEnv, jobject androidContext);
	virtual ~AndroidVM();
	jobject getAndroidContext();

private:
	jobject androidContext;
};

#endif
