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

#include "Env.h"

#include "defines.h"
#include <pthread.h>

#include "VM.h"
#ifndef ANDROID
# include "JREVM.h"
#endif
#include "ArrayConv.h"
#include "Conv.h"
#include "Interface.h"

using namespace bridge;

static pthread_key_t key;
static pthread_once_t key_once = PTHREAD_ONCE_INIT;

static void initProcess() {
  LOGV("Env::initProcess(): ent\n");
	pthread_key_create(&key, 0);
#ifndef ANDROID
	/* JRE VM */
	int result = JREVM::initProcess();
	if(result != OK) {
		/* fatal */
		LOGV("Fatal error in static initialisation of Env\n");
	}
#endif //ANDROID
  LOGV("Env::initProcess(): ret\n");
}

using namespace v8;

#ifdef ANDROID
/* preemptively called when starting a node instance
 * so the reactive initEnv() won't occur*/
void Env::setupEnv(VM *vm) {
  LOGV("Env::setupEnv(): ent\n");
  pthread_once(&key_once, initProcess);
  initThread(vm);
  LOGV("Env::setupEnv(): ret\n");
}
#endif

/* called on the first occasion that the Env is obtained in a process */
Env *Env::initThread(VM *vm) {
  LOGV("Env::initThread(): ent\n");
	Env *result = new Env(vm);
	pthread_setspecific(key, result);
  LOGV("Env::initThread(): ret\n");
	return result;
}

Env *Env::getEnv_nocheck() {
	Env *result = (Env *)pthread_getspecific(key);
  //LOGV("Env::getEnv_nocheck(): result = %p\n", result);
	return result;
}

Env *Env::getEnv() {
	pthread_once(&key_once, initProcess);
	Env *result;
	if((result = getEnv_nocheck()) == 0) {
		result = initThread(0);
	}
	return result;
}

Env::Env(VM *vm) {
  /* vm */
#ifndef ANDROID
  if(!vm)
    vm = new JREVM();
#endif
  this->vm = vm;
}

void Env::moduleLoaded() {
  if(moduleCount++ == 0)
    uv_async_init(nodeIsolate->Loop(), &async, asyncCb);
}

void Env::moduleUnloaded() {
  if(--moduleCount == 0)
    uv_close((uv_handle_t *)&async, 0);
}

Env::~Env() {
  LOGV("Env::~Env(): ent, this=%p\n", this);
  JNIEnv *jniEnv = vm->getJNIEnv();
  jniEnv->CallVoidMethod(jEnv, releaseMethodId);
  jniEnv->DeleteGlobalRef(jEnv);
  jniEnv->DeleteGlobalRef(jEnvClass);
  conv->dispose(jniEnv);
  delete conv;
  for(int i = 0; i < interfaces->getLength(); i++) {
    Interface *interface = interfaces->get(i);
    if(interface) interface->dispose(jniEnv);
  }
  delete interfaces;
	delete vm;
  LOGV("Env::~Env(): ret, this=%p\n", this);
}

int Env::init() {
  LOGV("Env::init(): ent, this=%p\n", this);
  int result = initV8();
  if(result == OK) {
    conv = new Conv(this, vm->getJNIEnv());
    interfaces = TArray<Interface*>::New();
    result = initJava();
    
    /* async */
    async.data = this;
    moduleCount = 0;
  }
  LOGV("Env::init(): ret\n");
  return result;
}

int Env::initV8() {
  /* node state */
  nodeIsolate = node::Isolate::GetCurrent();
  v8Isolate = v8::Isolate::GetCurrent();
  nodeIsolate->setExitHandler(Env::atExit);
  return OK;
}

int Env::initJava() {
  int result = OK;
  JNIEnv *jniEnv = vm->getJNIEnv();
  jEnvClass = (jclass)(jniEnv->NewGlobalRef(jniEnv->FindClass("org/meshpoint/anode/bridge/Env")));
  createMethodId = jniEnv->GetStaticMethodID(jEnvClass, "create", "(JLjava/lang/Object;)Lorg/meshpoint/anode/bridge/Env;");
  releaseMethodId = jniEnv->GetMethodID(jEnvClass, "release", "()V");
  jobject jEnvCtx;
  vm->createEnvContext(&jEnvCtx);
  jEnv = jniEnv->NewGlobalRef(jniEnv->CallStaticObjectMethod(jEnvClass, createMethodId, (jlong)this, jEnvCtx));
  loadMethodId = jniEnv->GetMethodID(jEnvClass, "loadModule", "(Ljava/lang/String;Lorg/meshpoint/anode/bridge/ModuleContext;)Ljava/lang/Object;");
  unloadMethodId = jniEnv->GetMethodID(jEnvClass, "unloadModule", "(Ljava/lang/String;)Z");
  onEntryMethodId = jniEnv->GetMethodID(jEnvClass, "onEntry", "()V");
  findClassMethodId = jniEnv->GetMethodID(jEnvClass, "findClass", "(Ljava/lang/Class;)I");
  
  if(jniEnv->ExceptionCheck()) {
    result = ErrorVM;
    jniEnv->ExceptionClear();
  }
  return result;
}

void Env::atExit() {
  delete getEnv_nocheck();
  pthread_setspecific(key, 0);
}

void Env::asyncCb(uv_async_t *async, int status) {
  Env *env = (Env *)async->data;
  env->vm->getJNIEnv()->CallVoidMethod(env->jEnv, env->onEntryMethodId);
}

void Env::setAsync() {
  uv_async_send(&async);
}
                
Local<Value> Env::load(Handle<String> moduleName, Handle<Object> moduleExports) {
  HandleScope scope;
	Local<Value> module;
  
  LOGV("Env::load(): ent\n");
  /* wrap the exports object */
  jobject jExports;
  JNIEnv *jniEnv = vm->getJNIEnv();
  int result = conv->ToJavaObject(jniEnv, moduleExports, TYPE_OBJECT, &jExports);
  if(result != OK) {
    LOGV("Fatal error: unable to convert module exports object\n");
    return Local<Value>(*Undefined());
  }
    
  /* convert the moduleName to jstring */
  jstring jModuleName;
  result = Conv::ToJavaString(jniEnv, moduleName, &jModuleName);
  if(result != OK) {
    LOGV("Fatal error: unable to convert modulename string\n");
    return Local<Value>(*Undefined());
  }

  /* create the module context */
  jobject jCtx;
  result = vm->createModuleContext(jEnv, jExports, &jCtx);
  if(result == OK) {
    jobject jModule = jniEnv->CallObjectMethod(jEnv, loadMethodId, jModuleName, jCtx);
    if(jModule) {
      /* wrap this according to its type */
      result = conv->ToV8Value(jniEnv, jModule, TYPE_NONE, &module);
    }
  }
  if(result == OK && !module.IsEmpty()) {
    moduleLoaded();
    return scope.Close(module);
  }

  conv->ThrowV8ExceptionForErrno(result);
  return Local<Value>(*Undefined());
}

Local<Value> Env::unload(Handle<String> moduleName) {
  HandleScope scope;

  JNIEnv *jniEnv = vm->getJNIEnv();
  jstring jModuleName;
  int result = Conv::ToJavaString(jniEnv, moduleName, &jModuleName);
  if(result == OK) {
    if(jniEnv->CallBooleanMethod(jEnv, unloadMethodId, jModuleName))
      moduleUnloaded();
  }

  return Local<Value>(*Undefined());
}
