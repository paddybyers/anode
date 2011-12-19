# Set up the following environment variables
# NODE_ROOT: location of the node root directory (for include files)
# ANODE_ROOT: location of the anode root directory (for the libjninode binary)

# Variable definitions for Android applications
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

# Modify this line to configure to the module name.
LOCAL_MODULE := hello

# Add any additional defines or compiler flags.
# Do not delete these existing flags as these are required
# for the included node header files.
LOCAL_CFLAGS := \
	-D__POSIX__ \
	-DBUILDING_NODE_EXTENSION \
	-include sys/select.h

LOCAL_CPPFLAGS :=

# Add any additional required directories for the include path.
# Do not delete these existing directories as these are required
# for the included node header files.
LOCAL_C_INCLUDES := $(NODE_ROOT)/src \
	$(NODE_ROOT)/deps/v8/include \
	$(NODE_ROOT)/deps/uv/include

# Add any additional required shared libraries that the addon depends on.
LOCAL_LDLIBS := \
	$(ANODE_ROOT)/libs/armeabi/libjninode.so

LOCAL_CPP_EXTENSION := .cc

LOCAL_SRC_FILES :=\
	src/hello.cc

LOCAL_STATIC_LIBRARIES := 

# Do not edit this line.
include $(ANODE_ROOT)/sdk/addon/build-node-addon.mk

