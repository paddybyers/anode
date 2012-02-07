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

#include "Bridge.h"

#include "defines.h"
#include "Env.h"

using namespace v8;
using namespace bridge;

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
  Env::getEnv_nocheck()->unload(moduleName);
  return Undefined();
}

void init(Handle<Object> target) {
  int result = Env::getEnv()->init();
  if(result == OK) {
    target->Set(String::NewSymbol("load"), FunctionTemplate::New(Load)->GetFunction());
    target->Set(String::NewSymbol("unload"), FunctionTemplate::New(Unload)->GetFunction());
    return;
  }
  LOGV("Fatal error in Bridge::init(): errno = %d\n", result);
}

NODE_MODULE(bridge, init);
