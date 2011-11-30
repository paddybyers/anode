#include "Env.h"

#include "defines.h"
#include <pthread.h>

#include "VM.h"
#ifndef ANDROID
# include "JREVM.h"
#endif

static pthread_key_t key;
static pthread_once_t key_once = PTHREAD_ONCE_INIT;
static void static_init() {
	pthread_key_create(&key, 0);
#ifndef ANDROID
	/* JRE VM */
	int result = JREVM::static_init();
	if(result != OK) {
		/* fatal */
		fprintf(stderr, "Fatal error in static initialisation of Env\n");
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
Env *Env::initEnv(VM *vm) {
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
		result = initEnv(0);
	}
	return result;
}

Env::Env(VM *vm) {
	nodeIsolate = node::Isolate::GetCurrent();
	v8Isolate = v8::Isolate::GetCurrent();
#ifndef ANDROID
	if(!vm)
		vm = new JREVM();
#endif
	this->vm = vm;
  initEnv(nodeIsolate, v8Isolate);
}

Env::~Env() {
  JNIEnv *jniEnv = vm->getJNIEnv();
  jniEnv->CallVoidMethod(jEnv, releaseMethodId);
  jniEnv->DeleteGlobalRef(jEnv);
  jniEnv->DeleteGlobalRef(jEnvClass);
	delete vm;
}

int Env::initEnv(node::Isolate *nodeIsolate, v8::Isolate *v8Isolate) {
  int result = OK;
  nodeIsolate->exitHandler = Env::atExit;
  JNIEnv *jniEnv = vm->getJNIEnv();
  jEnvClass = (jclass)(jniEnv->NewGlobalRef(jniEnv->FindClass("org/meshpoint/anode/bridge/Env")));
  createMethodId = jniEnv->GetStaticMethodID(jEnvClass, "create", "(JJ)Lorg/meshpoint/anode/bridge/Env;");
  releaseMethodId = jniEnv->GetMethodID(jEnvClass, "release", "()V");
  jEnv = jniEnv->NewGlobalRef(jniEnv->CallStaticObjectMethod(jEnvClass, createMethodId,  (jlong)nodeIsolate,  (jlong)v8Isolate));
  loadMethodId = jniEnv->GetMethodID(jEnvClass, "loadModule", "(Ljava/lang/String;Lorg/meshpoint/anode/bridge/ModuleContext;)Lorg/meshpoint/anode/type/IValue;");
  
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
  
Local<Value> Env::load(Handle<String> moduleName, Handle<Object> moduleExports) {
  HandleScope scope;
	Local<Value> module;
  
  /* wrap the exports object */
  jobject jCtx;
  jobject jExports = 0;
  JNIEnv *jniEnv = vm->getJNIEnv();
  
  /* convert the moduleName to jstring */
  int nameLen = moduleName->Length();
  jchar *nameBuf = new jchar[nameLen];
  if(!nameBuf) {
    fprintf(stderr, "Fatal: memory allocation failure (load())\n");
    return module;
  }
  moduleName->Write(nameBuf, 0, nameLen, v8::String::NO_NULL_TERMINATION);
  jstring jModuleName = jniEnv->NewString(nameBuf, nameLen);
  delete[] nameBuf;

  /* create the module context */
  int result = vm->createContext(jEnv, jExports, &jCtx);
  if(result == OK) {
    jobject jModule = jniEnv->CallObjectMethod(jEnv, loadMethodId, jModuleName, jCtx);
    if(jModule) {
      /* wrap this according to its type */
    }
  }
	return scope.Close(module);
}
