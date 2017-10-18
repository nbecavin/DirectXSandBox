#----------------------------------------------------------------------------------
# File:            apps\lifecycle_app\jni\Android.mk
# Samples Version: NVIDIA Android Lifecycle samples 1_0beta 
# Email:           tegradev@nvidia.com
# Web:             http://developer.nvidia.com/category/zone/mobile-development
#
# Copyright 2009-2011 NVIDIA® Corporation 
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#----------------------------------------------------------------------------------

MY_LOCAL_PATH := $(call my-dir)
LOCAL_PATH := $(MY_LOCAL_PATH)

include $(CLEAR_VARS)

LOCAL_MODULE := SandBoxDroidNativeLib

#NVidia libs
LOCAL_SRC_FILES := $(wildcard nv_apk_file/*.c)
LOCAL_SRC_FILES += $(wildcard nv_event/*.cpp)
LOCAL_SRC_FILES += $(wildcard nv_file/*.c)
LOCAL_SRC_FILES += $(wildcard nv_glesutil/*.cpp)
LOCAL_SRC_FILES += $(wildcard nv_hhdds/*.cpp)
LOCAL_SRC_FILES += $(wildcard nv_math/*.cpp)
LOCAL_SRC_FILES += $(wildcard nv_shader/*.cpp)
LOCAL_SRC_FILES += $(wildcard nv_log/*.cpp)
LOCAL_SRC_FILES += $(wildcard nv_sound/*.cpp)
LOCAL_SRC_FILES += $(wildcard nv_thread/*.c)
LOCAL_SRC_FILES += $(wildcard nv_time/*.cpp)
LOCAL_SRC_FILES += $(wildcard nv_util/*.cpp)
#SandBox sources
LOCAL_SRC_FILES += $(wildcard ../../SandBox/*.cpp)
LOCAL_SRC_FILES += $(wildcard *.cpp)
LOCAL_SRC_FILES += $(wildcard *.c)

LOCAL_CFLAGS = -fms-extensions -D_ANDROID -D_DEBUG -include "../../SandBox/stdafx.h"

LOCAL_LDFLAGS := -Wl,--no-undefined              

LOCAL_ARM_MODE   := arm
LOCAL_LDFLAGS += -Lobj/local/$(TARGET_ARCH_ABI)/ -L../../../libs/obj/local/$(TARGET_ARCH_ABI)
LOCAL_LDFLAGS += -L../libs/nv_obj -L../NvLibs/tegra/lib

LOCAL_C_INCLUDES := ../NvLibs/jni/ ../NvLibs/tegra/include
LOCAL_C_INCLUDES += ../../SandBox/Includes/

LOCAL_LDLIBS := -lstdc++ -lc -lm -llog -ldl -lGLESv2
#LOCAL_LDLIBS += -lnvmath -lnvshader -lnvfile -lnvapkfile -lnvevent -lnvtime -lEGL -lnvthread
LOCAL_LDLIBS += -lEGL

include $(BUILD_SHARED_LIBRARY)
