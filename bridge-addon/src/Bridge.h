#ifndef BRIDGE_BRIDGE_H
#define BRIDGE_BRIDGE_H

#include <node.h>

v8::Handle<v8::Value> Load(const v8::Arguments& args);
void RegisterModule(v8::Handle<v8::Object> target);

#endif
