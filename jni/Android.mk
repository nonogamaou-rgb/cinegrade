LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := CineGrade
LOCAL_SRC_FILES := main.cpp
LOCAL_C_INCLUDES := $(AML_INC)
LOCAL_LDLIBS := -llog -ldl
include $(BUILD_SHARED_LIBRARY)
