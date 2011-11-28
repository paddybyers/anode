# Set up the following environment variables
# NODE_ROOT: location of the node root directory (for include files)
# ANODE_ROOT: location of the anode root directory (for the libjninode binary)

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := bridge

LOCAL_CFLAGS := \
	-D__POSIX__ \
	-DBUILDING_NODE_EXTENSION

LOCAL_CPPFLAGS :=

LOCAL_C_INCLUDES := \
	src \
	$(NODE_ROOT)/src \
	$(NODE_ROOT)/deps/v8/include \
	$(NODE_ROOT)/deps/uv/include

LOCAL_LDLIBS := \
	$(ANODE_ROOT)/libnode/libs/armeabi/libjninode.so

LOCAL_CPP_EXTENSION := .cc

LOCAL_SRC_FILES :=\
	src/bridge.cpp

LOCAL_STATIC_LIBRARIES := 

# Do not edit this line.
include $(ANODE_ROOT)/sdk/addon/build-node-addon.mk

