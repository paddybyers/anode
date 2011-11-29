#ifndef BRIDGE_ENV_H
#define BRIDGE_ENV_H

#include "defines.h"
#include <node.h>
#include <jni.h>

class VM;

using namespace v8;

class LIB_EXPORT Env {
public:
#ifdef ANDROID
	static LIB_EXPORT void setupEnv(VM *vm);
#endif
	static LIB_EXPORT Env *getEnv();
	static LIB_EXPORT Env *getEnv_nocheck();
	LIB_EXPORT Local<Value> load(Handle<String> moduleName);

private:
	static Env *initEnv(VM *vm);
	Env(VM *vm);
	~Env();

	node::Isolate *nodeIsolate;
	v8::Isolate   *v8Isolate;
	VM            *vm;
	
};

#endif
