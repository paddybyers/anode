ANODE_ROOT       := $(call my-dir)

ifndef NODE_ROOT
	NODE_ROOT    := $(ANODE_ROOT)/../node
endif

APP_PROJECT_PATH := $(ANODE_ROOT)
APP_MODULES      := jninode bridge
APP_BUILD_SCRIPT := $(APP_PROJECT_PATH)/Android.mk
APP_PLATFORM     := android-14
APP_ABI          := armeabi
NDK_MODULE_PATH  := $(NDK_MODULE_PATH):$(ANODE_ROOT):$(ANODE_ROOT)/..:$(NODE_ROOT):$(NODE_ROOT)/..
