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

#ifndef BRIDGE_VM_H
#define BRIDGE_VM_H

#include "defines.h"

namespace bridge {

class VM {
public:
	VM();
	virtual    ~VM();
	JNIEnv     *getJNIEnv();
  virtual int createContext(jobject jEnv, jobject jExports, jobject *jCtx) = 0;

protected:
	JNIEnv     *jniEnv;
  /* module lifecycle */
  jclass      jContextClass;
  jmethodID   createContextMethodId;
};

} // namespace bridge
#endif
