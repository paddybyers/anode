# Set up the following environment variables
# NODE_ROOT: location of the node root directory (for include files)
# ANODE_ROOT: location of the anode root directory (for the libjninode binary)

# Variable definitions for Android applications
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := hello

LOCAL_CFLAGS := \
    -D__POSIX__ \
	-DBUILDING_NODE_EXTENSION

LOCAL_CPPFLAGS :=

LOCAL_C_INCLUDES := $(NODE_ROOT)/src \
	$(NODE_ROOT)/deps/v8/include \
	$(NODE_ROOT)/deps/uv/include

LOCAL_LDLIBS := \
	$(ANODE_ROOT)/libnode/libs/armeabi/libjninode.so

LOCAL_CPP_EXTENSION := .cc

LOCAL_SRC_FILES :=\
	src/hello.cc

LOCAL_STATIC_LIBRARIES := 

include $(ANODE_ROOT)/sdk/addon/build-node-addon.mk

