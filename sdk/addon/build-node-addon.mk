LOCAL_BUILD_SCRIPT := $(BUILD_SYSTEM)/build-node-addon.mk
LOCAL_MAKEFILE     := $(local-makefile)

$(call check-defined-LOCAL_MODULE,$(LOCAL_BUILD_SCRIPT))
$(call check-LOCAL_MODULE,$(LOCAL_MAKEFILE))
$(call check-LOCAL_MODULE_FILENAME)

# we are building target objects
my := TARGET_

$(call handle-module-filename,,.node)
$(call handle-module-built)

LOCAL_MODULE_CLASS := SHARED_LIBRARY

include $(BUILD_SYSTEM)/build-module.mk
ALL_SHARED_LIBRARIES += $(LOCAL_BUILT_MODULE)
