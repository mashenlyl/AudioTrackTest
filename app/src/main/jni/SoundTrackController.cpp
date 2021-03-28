//
// Created by jetson on 2021/3/13.
//
#include "com_example_audiotracktest_opensles_SoundTrackController.h"
#include "libmedia/sound_service.h"
#include "libcommon/CommonTools.h"

#define LOG_TAG "liyaoliSoundTrack"

SoundService* soundService = NULL;
JNIEXPORT jboolean JNICALL Java_com_example_audiotracktest_opensles_SoundTrackController_setAudioDataSource
        (JNIEnv *env, jobject obj, jstring acPathParam) {
    const char* acPath = env->GetStringUTFChars(acPathParam, NULL);
    soundService = SoundService::GetInstance();
    soundService->initSongDecoder(acPath);
    SLresult result = soundService->initSoundTrack();
    env->ReleaseStringUTFChars(acPathParam, acPath);
    return true;
}


JNIEXPORT void JNICALL Java_com_example_audiotracktest_opensles_SoundTrackController_play
(JNIEnv * env, jobject obj) {
    if (NULL != soundService) {
        soundService->play();
    }
}



JNIEXPORT void JNICALL Java_com_example_audiotracktest_opensles_SoundTrackController_stop
(JNIEnv * env, jobject obj) {
    if (NULL != soundService) {
        soundService->stop();
        soundService = NULL;
    }
}
























