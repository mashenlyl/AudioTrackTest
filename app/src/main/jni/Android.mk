LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CFLAGS += -D__STDC_CONSTANT_MACROS
LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/libcommon \
	$(LOCAL_PATH)/libmusicdecoder \
    $(LOCAL_PATH)/3rdparty/ffmpeg/include \
    $(LOCAL_PATH)/libmedia

LOCAL_SRC_FILES = \
MusicDecoder.cpp \
SoundTrackController.cpp

LOCAL_STATIC_LIBRARIES := libcommontool libmusicdecoder libmedia

LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -llog
LOCAL_LDLIBS += -lOpenSLES
LOCAL_LDLIBS += -lz
LOCAL_LDLIBS += -landroid

LOCAL_LDFLAGS += $(LOCAL_PATH)/3rdparty/prebuilt/x86/libavfilter.a
LOCAL_LDFLAGS += $(LOCAL_PATH)/3rdparty/prebuilt/x86/libavformat.a
LOCAL_LDFLAGS += $(LOCAL_PATH)/3rdparty/prebuilt/x86/libavcodec.a
LOCAL_LDFLAGS += $(LOCAL_PATH)/3rdparty/prebuilt/x86/libavutil.a
LOCAL_LDFLAGS += $(LOCAL_PATH)/3rdparty/prebuilt/x86/libswscale.a
LOCAL_LDFLAGS += $(LOCAL_PATH)/3rdparty/prebuilt/x86/libswresample.a

LOCAL_MODULE := libsongstudio
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/libcommon \
    $(LOCAL_PATH)/3rdparty/ffmpeg/include

LOCAL_CFLAGS := -DHAVE_CONFIG_H -ffast-math

LOCAL_SRC_FILES += \
./libmusicdecoder/accompany_decoder_controller.cpp \
 ./libmusicdecoder/accompany_decoder.cpp

LOCAL_MODULE := libmusicdecoder
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_SRC_FILES += \
./libcommon/packet_queue.cpp \
./libcommon/packet_pool.cpp

LOCAL_MODULE := libcommontool
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_STATIC_LIBRARIES := libmusicdecoder

LOCAL_CFLAGS := -DHAVE_CONFIG_H -ffast-math -O3

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/3rdparty/ffmpeg/include \
	$(LOCAL_PATH)/libcommon

LOCAL_SRC_FILES += \
	./libmedia/sound_service.cpp \
	./libmedia/opensl_es_context.cpp

LOCAL_LDLIBS += -lOpenSLES

LOCAL_MODULE := libmedia
include $(BUILD_STATIC_LIBRARY)





