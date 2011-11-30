#ifndef BRIDGE_ANDROIDVM_H
#define BRIDGE_ANDROIDVM_H

#include "VM.h"

class AndroidVM : public VM {
public:
	AndroidVM(JNIEnv *jniEnv, jobject jAndroidContext);
	virtual ~AndroidVM();
	virtual int createContext(jobject jEnv, jobject jExports, jobject *jCtx);

private:
	jobject jAndroidContext;
};

#endif
