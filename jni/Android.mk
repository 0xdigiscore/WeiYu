LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := read
LOCAL_SRC_FILES := main2.cpp
LOCAL_LDLIBS += -llog
include $(BUILD_SHARED_LIBRARY)
