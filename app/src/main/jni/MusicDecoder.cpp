//
// Created by jetson on 2021/3/8.
//

#include "com_example_audiotracktest_audiotrack_MusicDecoder.h"
#include "accompany_decoder_controller.h"

#define LOG_TAG = "MusicDecoder"

AccompanyDecoderController* mDecoderController = NULL;

JNIEXPORT jint JNICALL Java_com_example_audiotracktest_audiotrack_MusicDecoder_openFile
    (JNIEnv *env, jobject obj, jstring acFilePathParam) {
    const char * acFilePath = env->GetStringUTFChars(acFilePathParam, NULL);
    mDecoderController = new AccompanyDecoderController();
    mDecoderController->init(acFilePath);
    env->ReleaseStringUTFChars(acFilePathParam, acFilePath);
    return 0;
}



JNIEXPORT jint JNICALL Java_com_example_audiotracktest_audiotrack_MusicDecoder_readSamples
        (JNIEnv * env, jobject obj, jshortArray array, jint size, jintArray extraSlientSampleSize) {
    if (NULL != mDecoderController) {
        jshort* target = env->GetShortArrayElements(array, 0);
        jint* slientSizeArr = env->GetIntArrayElements(extraSlientSampleSize, 0);
        int result = mDecoderController->readSamples(target, size, slientSizeArr);
        env->ReleaseIntArrayElements(extraSlientSampleSize, slientSizeArr, 0);
        env->ReleaseShortArrayElements(array, target, 0);
        return result;
    }
    return -1;
}


JNIEXPORT void JNICALL Java_com_example_audiotracktest_audiotrack_MusicDecoder_closeFile
(JNIEnv * env, jobject obj) {
    if (NULL != mDecoderController) {
        mDecoderController->destroy();
        delete mDecoderController;
        mDecoderController = NULL;
    }
}





