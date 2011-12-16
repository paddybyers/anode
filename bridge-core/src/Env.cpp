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
#include "Utils.cpp"

using namespace bridge;

static pthread_key_t key;
static pthread_once_t key_once = PTHREAD_ONCE_INIT;
static void static_init() {
	pthread_key_create(&key, 0);
#ifndef ANDROID
	/* JRE VM */
	int result = JREVM::static_init();
	if(result != OK) {
		/* fatal */
		LOGV("Fatal error in static initialisation of Env\n");
	}
#endif //ANDROID
}

using namespace v8;

#ifdef ANDROID
/* preemptively called when starting a node instance
 * so the reactive initEnv() won't occur*/
void Env::setupEnv(VM *vm) {
  static_init();
	initEnv(vm);
}
#endif

/* called on the first occasion that the Env is obtained in a process */
Env *Env::initOnce(VM *vm) {
	Env *result = new Env(vm);
	pthread_setspecific(key, result);
	return result;
}

Env *Env::getEnv_nocheck() {
	return (Env *)pthread_getspecific(key);
}

Env *Env::getEnv() {
	pthread_once(&key_once, static_init);
	Env *result;
	if((result = getEnv_nocheck()) == 0) {
		result = initOnce(0);
	}
	return result;
}

Env::Env(VM *vm) {
  /* node state */
	nodeIsolate = node::Isolate::GetCurrent();
	v8Isolate = v8::Isolate::GetCurrent();
  
  /* java state */
#ifndef ANDROID
	if(!vm)
		vm = new JREVM();
#endif
	this->vm = vm;
  conv = new Conv(this, vm->getJNIEnv());
  interfaces = TArray<Interface*>::New();
  initJava(nodeIsolate);
  
  /* async */
  async.data = this;
  moduleCount = 0;
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
}

int Env::initJava(node::Isolate *nodeIsolate) {
  int result = OK;
  nodeIsolate->exitHandler = Env::atExit;
  JNIEnv *jniEnv = vm->getJNIEnv();
  jEnvClass = (jclass)(jniEnv->NewGlobalRef(jniEnv->FindClass("org/meshpoint/anode/bridge/Env")));
  createMethodId = jniEnv->GetStaticMethodID(jEnvClass, "create", "(J)Lorg/meshpoint/anode/bridge/Env;");
  releaseMethodId = jniEnv->GetMethodID(jEnvClass, "release", "()V");
  jEnv = jniEnv->NewGlobalRef(jniEnv->CallStaticObjectMethod(jEnvClass, createMethodId,  (jlong)this));
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
  result = vm->createContext(jEnv, jExports, &jCtx);
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
    jniEnv->CallObjectMethod(jEnv, unloadMethodId, jModuleName);
    moduleUnloaded();
  }

  return Local<Value>(*Undefined());
}
