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

#ifndef BRIDGE_BRIDGE_H
#define BRIDGE_BRIDGE_H

#include <node.h>

v8::Handle<v8::Value> Load(const v8::Arguments& args);
v8::Handle<v8::Value> Unload(const v8::Arguments& args);

extern "C" NODE_EXTERN void init(v8::Handle<v8::Object> target);

#endif
