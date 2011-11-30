#include "Env.h"

#include "defines.h"
#include <pthread.h>

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
	/* FIXME: add atexit call here */
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
}

Env::~Env() {
	delete vm;
}

Local<Value> Env::load(Handle<String> moduleName) {
	Local<Value> result;
	return result;
}
