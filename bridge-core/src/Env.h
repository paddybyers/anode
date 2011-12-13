#ifndef BRIDGE_ENV_H
#define BRIDGE_ENV_H

#include "defines.h"
#include <node.h>
#ifdef __APPLE__
# include <JavaVM/jni.h>
#else
# include <jni.h>
#endif

#include "Interface.h"
#include "Utils.h"

class VM;
class Conv;

using namespace v8;

class LIB_EXPORT Env {
public:
#ifdef ANDROID
	static LIB_EXPORT void setupEnv(VM *vm);
#endif
	static LIB_EXPORT Env *getEnv();
	static LIB_EXPORT Env *getEnv_nocheck();
	LIB_EXPORT Local<Value> load(Handle<String> moduleName, Handle<Object> moduleExports);
  inline Interface *getInterface(classId class_) {return interfaces->get(Interface::classId2Idx(class_));}

private:
	Env(VM *vm);
	~Env();
  int initJava(node::Isolate *nodeIsolate);
  static void atExit();

	static Env         *initEnv(VM *vm);
	node::Isolate      *nodeIsolate;
	v8::Isolate        *v8Isolate;
	VM                 *vm;
  Conv               *conv;
  TArray<Interface*> *interfaces;
  
  uv_async_t         async;

  /* JNI */
	jclass             jEnvClass;
  jobject            jEnv;
	jmethodID          createMethodId;
	jmethodID          releaseMethodId;
  jmethodID          loadMethodId;
};

#endif
