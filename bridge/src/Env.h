/*
 * Copyright 2011-2012 Paddy Byers
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */

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
#include "Utils-inl.h"

namespace bridge {

class Conv;
class VM;

class Env {
public:
#ifdef ANDROID
	static void setupEnv(VM *vm);
#endif
	static Env *getEnv();
	static Env *getEnv_nocheck();
  int init();
  v8::Local<v8::Value> load(v8::Handle<v8::String> moduleName, v8::Handle<v8::Object> moduleExports);
  v8::Local<v8::Value> unload(v8::Handle<v8::String> moduleName);
  inline Conv *getConv() {return conv;}
  inline Interface *getInterface(classId class_) {return interfaces->get(Interface::classId2Idx(class_));}
  inline void putInterface(classId class_, Interface *interface) {interfaces->put(Interface::classId2Idx(class_), interface);}
  inline VM *getVM() {return vm;}
  void setAsync();

private:
	Env(VM *vm);
	~Env();
  int initV8();
  int initJava();
  static void atExit();
  static void asyncCb(uv_async_t *async, int status);
  
  void               moduleLoaded();
  void               moduleUnloaded();
  int                moduleCount;  
  uv_async_t         async;

	static Env         *initThread(VM *vm);
	node::Isolate      *nodeIsolate;
	v8::Isolate        *v8Isolate;
	VM                 *vm;
  Conv               *conv;
  TArray<Interface*> *interfaces;

  /* JNI */
	jclass             jEnvClass;
  jobject            jEnv;
	jmethodID          createMethodId;
	jmethodID          releaseMethodId;
  jmethodID          loadMethodId;
  jmethodID          unloadMethodId;
  jmethodID          onEntryMethodId;
  jmethodID          findClassMethodId;
  
  friend class Conv;
};

} //namespace bridge
#endif
