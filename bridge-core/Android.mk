# Set up the following environment variables
# NODE_ROOT: location of the node root directory (for include files)
# ANODE_ROOT: location of the anode root directory (for the libjninode binary)

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := bridge-core

LOCAL_CFLAGS := \
	-D__POSIX__ \
	-include sys/select.h

LOCAL_CPPFLAGS :=

LOCAL_C_INCLUDES := \
	src \
	$(NODE_ROOT)/src \
	$(NODE_ROOT)/deps/v8/include \
	$(NODE_ROOT)/deps/uv/include

LOCAL_CPP_EXTENSION := .cpp

LOCAL_SRC_FILES := \
	src/AndroidVM.cpp \
	src/ArrayConv.cpp \
	src/Conv.cpp \
	src/Env.cpp \
	src/Interface.cpp \
	src/Utils.cpp \
	src/VM.cpp

LOCAL_STATIC_LIBRARIES := 

include $(BUILD_STATIC_LIBRARY)
