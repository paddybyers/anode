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

#include "AndroidVM.h"

using namespace bridge;

AndroidVM::AndroidVM(JNIEnv *jniEnv, jobject jAndroidContext) : VM() {
	this->jniEnv = jniEnv;
	this->jAndroidContext = jniEnv->NewGlobalRef(jAndroidContext);
	jContextClass = (jclass)jniEnv->NewGlobalRef(jniEnv->FindClass("org/meshpoint/anode/AndroidContext"));
	createContextMethodId = jniEnv->GetMethodID(jContextClass, "<init>", "(Lorg/meshpoint/anode/bridge/Env;Lorg/meshpoint/anode/js/JSObject;Landroid/content/Context;)V");
}

AndroidVM::~AndroidVM() {
    jniEnv->DeleteGlobalRef(jContextClass);
    jniEnv->DeleteGlobalRef(jAndroidContext);
}

int AndroidVM::createEnvContext(jobject *jCtx) {
	int result = OK;
	*jCtx = jAndroidContext;
	return result;
}

int AndroidVM::createModuleContext(jobject jEnv, jobject jExports, jobject *jCtx) {
	int result = OK;
	*jCtx = jniEnv->NewObject(jContextClass, createContextMethodId, jEnv, jExports, jAndroidContext);
	if(jniEnv->ExceptionCheck()) {
		result = ErrorVM;
		jniEnv->ExceptionClear();
	}
	return result;
}
