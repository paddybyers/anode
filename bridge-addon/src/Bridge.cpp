#include "Bridge.h"

#include <defines.h>
#include "Env.h"

using namespace v8;

/*
 * Creates an instance of a java module, specified by name.
 */
Handle<Value> Load(const Arguments& args) {
  HandleScope scope;
  
  /* Check arguments */
  if (args.Length() < 2) {
    return ThrowException(
                          Exception::TypeError(String::New("bridge.load(): error: no modulName argument"))
                          );
  }
  
	if(!args[0]->IsString()) {
    return ThrowException(
                          Exception::TypeError(String::New("bridge.load(): error: moduleName argument must be a String"))
                          );
	}
  
	if(!args[1]->IsObject()) {
    return ThrowException(
                          Exception::TypeError(String::New("bridge.load(): error: moduleExports argument must be an Object"))
                          );
	}
  
  Local<String> moduleName = args[0]->ToString();
  Local<Object> moduleExports = args[1]->ToObject();
  return scope.Close(bridge::Env::getEnv_nocheck()->load(moduleName, moduleExports));
}

/*
 * Releases an instance of a java module, specified by name.
 */
Handle<Value> Unload(const Arguments& args) {
  HandleScope scope;
  
  /* Check arguments */
  if (args.Length() < 1) {
    return ThrowException(
                          Exception::TypeError(String::New("bridge.unload(): error: no modulName argument"))
                          );
  }
  
	if(!args[0]->IsString()) {
    return ThrowException(
                          Exception::TypeError(String::New("bridge.unload(): error: moduleName argument must be a String"))
                          );
	}
  
  Local<String> moduleName = args[0]->ToString();
  bridge::Env::getEnv_nocheck()->unload(moduleName);
  return Undefined();
}

void init(Handle<Object> target) {
  bridge::Env::getEnv();
  target->Set(String::NewSymbol("load"), FunctionTemplate::New(Load)->GetFunction());
  target->Set(String::NewSymbol("unload"), FunctionTemplate::New(Unload)->GetFunction());
}

NODE_MODULE(bridge, init);
