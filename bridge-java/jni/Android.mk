# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := jnibridge

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH) \
	$(ANODE_ROOT)/bridge-core/src \
	$(NODE_ROOT)/src \
	$(NODE_ROOT)/deps/v8/include \
	$(NODE_ROOT)/deps/uv/include

LOCAL_CFLAGS += \
	-D__POSIX__ \
	-DBUILDING_NODE_EXTENSION \
	-include sys/select.h
		
LOCAL_SRC_FILES += \
	src/org_meshpoint_anode_bridge_BridgeNative.cpp

LOCAL_STATIC_LIBRARIES := \
	bridge-core

LOCAL_SHARED_LIBRARIES :=

LOCAL_LDLIBS += \
	$(ANODE_ROOT)/libnode/libs/armeabi/libjninode.so \
	-llog

include $(BUILD_SHARED_LIBRARY)

$(call import-module,bridge-core)
