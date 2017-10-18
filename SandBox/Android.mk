
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := SandBoxNativeLib

LOCAL_SRC_FILES := nv_math.cpp nv_matrix.cpp nv_quat.cpp

include $(BUILD_STATIC_LIBRARY)
