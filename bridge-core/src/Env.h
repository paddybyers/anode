#ifndef BRIDGE_ENV_H
#define BRIDGE_ENV_H

#include "defines.h"
#include <node.h>
#ifdef __APPLE__
# include <JavaVM/jni.h>
#else
# include <jni.h>
#endif

class VM;
class V8ToJava;

using namespace v8;

class LIB_EXPORT Env {
public:
#ifdef ANDROID
	static LIB_EXPORT void setupEnv(VM *vm);
#endif
	static LIB_EXPORT Env *getEnv();
	static LIB_EXPORT Env *getEnv_nocheck();
	LIB_EXPORT Local<Value> load(Handle<String> moduleName, Handle<Object> moduleExports);

private:
	Env(VM *vm);
	~Env();
  int initEnv(node::Isolate *nodeIsolate, v8::Isolate *v8Isolate);
  static void atExit();

	static Env    *initEnv(VM *vm);
	node::Isolate *nodeIsolate;
	v8::Isolate   *v8Isolate;
	VM            *vm;
  V8ToJava      *v8ToJava;

  /* JNI */
	jclass        jEnvClass;
  jobject       jEnv;
	jmethodID     createMethodId;
	jmethodID     releaseMethodId;
  jmethodID     loadMethodId;
};

#endif
