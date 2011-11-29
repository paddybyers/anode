#ifndef BRIDGE_VM_H
#define BRIDGE_VM_H

#ifdef __APPLE__
# include <JavaVM/jni.h>
#else
# include <jni.h>
#endif //DARWIN

class VM {
public:
	VM();
	virtual ~VM();
	JNIEnv *getJNIEnv();

protected:
	JNIEnv *  jniEnv;
	jclass    envClass;
	jmethodID loadMethodId;
	jmethodID releaseMethodId;
};

#endif
