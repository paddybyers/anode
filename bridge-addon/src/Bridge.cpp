#include "Bridge.h"

#include "defines.h"
#include "Env.h"

using namespace v8;

/*
 * Creates an instance of a java module, specified by name.
 */
Handle<Value> Load(const Arguments& args) {
    HandleScope scope;

    /* Check arguments */
    if (args.Length() < 1) {
        return ThrowException(
            Exception::TypeError(String::New("bridge.load(): error: no modulName argument"))
        );
    }

	if(!args[0]->IsString()) {
        return ThrowException(
            Exception::TypeError(String::New("bridge.load(): error: moduleName argument must be a String"))
        );
	}

    Local<String> moduleName = args[0]->ToString();
    Env::getEnv_nocheck()->load(moduleName);
    return scope.Close(Env::getEnv_nocheck()->load(moduleName));
}

void RegisterModule(Handle<Object> target) {
	Env::getEnv();
    target->Set(String::NewSymbol("load"),
        FunctionTemplate::New(Load)->GetFunction());
}

NODE_MODULE(modulename, RegisterModule);
