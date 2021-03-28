//
// Created by jetson on 2021/3/13.
//

#include "sound_service.h"
#include "CommonTools.h"

#define LOG_TAG "liyaoliSoundService"

SoundService::SoundService() {
    LOGI("SoundService::SoundService()");
    mPlayingState = PLAYING_STATE_STOPPED;
}

SoundService::~SoundService() {
    LOGI("SoundService::~SoundService()");
}

SoundService* SoundService::mInstance = new SoundService();

SoundService* SoundService::GetInstance() {
    return mInstance;
}

void SoundService::producePacket() {
    LOGI("SoundService::producePacket() audio player call back method...");
    byte *audioBuffer = mBuffer + (mCurrentFrame % mBufferNums) * mPacketBufferSize;
    int result = 0;
    if (NULL != mDecoderController) {
        /*for (int j = 0; j < 33; j++) {
            int pSize = -1;
            pSize = mDecoderController->readSamples(mTarget + result, mPacketBufferSize/2, NULL);
            if (0 < pSize) {
                result += pSize;
            } else {
                break;
            }
        } */
        result = mDecoderController->readSamples(mTarget + result, mPacketBufferSize/2, NULL);
        LOGI("enter SoundService::producePacket() PLAYING_STATE_PLAYING packetBufferSize=%d, result=%d", mPacketBufferSize, result);
    }
    if (0 < result) {
        convertByteArrayFromShortArray(mTarget, result, audioBuffer);
        (*mAudioPlayerBufferQueue)->Enqueue(mAudioPlayerBufferQueue, audioBuffer, result * 2);
    }
    mCurrentFrame = (mCurrentFrame + 1) % mBufferNums;
}

SLresult SoundService::RegisterPlayerCallback() {
    return (*mAudioPlayerBufferQueue)->RegisterCallback(mAudioPlayerBufferQueue, PlayerCallback, this);
}

SLresult SoundService::stop() {
    LOGI("enter SoundService::stop()");
    mPlayingState = PLAYING_STATE_STOPPED;
    LOGI("Set the audio player state paused");
    SLresult result = SetAudioPlayerStateStoped();
    if (SL_RESULT_SUCCESS != result) {
        LOGI("Set the audio player state paused return false");
        return result;
    }
    DestroyContext();
    LOGI("out SoundService::stop()");
}

SLresult SoundService::play() {
    LOGI("enter SoundService::play()...");
    SLresult result = SetAudioPlayerStatePlaying();
    if (SL_RESULT_SUCCESS != result) {
        return result;
    }
    mPlayingState = PLAYING_STATE_PLAYING;

        int ret = 0;
        if (NULL != mDecoderController) {
            /*for (int j = 0; j < 33; j++) {
                int pSize = -1;
                pSize = mDecoderController->readSamples(mTarget + ret, mPacketBufferSize/2, NULL);
                if (0 < pSize) {
                    ret += pSize;
                } else {
                    break;
                }
                LOGI("ret = %d", ret);
            } */
            ret = mDecoderController->readSamples(mTarget + ret, mPacketBufferSize/2, NULL);
        }
        if (0 < ret) {
            convertByteArrayFromShortArray(mTarget, ret, mBuffer);
            (*mAudioPlayerBufferQueue)->Enqueue(mAudioPlayerBufferQueue, mBuffer, ret * 2);
        }
    LOGI("out SoundService::play()...");
    while(mPlayingState != PLAYING_STATE_STOPPED) {};
}

void SoundService::initSongDecoder(const char *acPath) {
    LOGI("enter SoundService::initSongDecoder");
    mDecoderController = new AccompanyDecoderController();
    mDecoderController->init(acPath);
    this->mBufferNums = 2;
    this->mCurrentFrame = 0;
    mPacketBufferSize = 2 * 44100 * 2;
    mBufferSize = mPacketBufferSize * mBufferNums;
}

SLresult SoundService::initSoundTrack() {
    LOGI("enter SoundService::initSoundTrack");
    SLresult result;
    OpenSLESContext* openSLESContext = OpenSLESContext::GetInstance();
    mEngine = openSLESContext->getEngine();

    result = CreateOutputMix();
    if (SL_RESULT_SUCCESS != result) {
        return result;
    }

    result = RealizeObject(mOutputMixObject);
    if (SL_RESULT_SUCCESS != result) {
        return result;
    }

    InitPlayerBuffer();
    result = CreateBufferQueueAudioPlayer();
    if (SL_RESULT_SUCCESS != result) {
        return result;
    }

    result = RealizeObject(mAudioPlayerObject);
    if (SL_RESULT_SUCCESS != result) {
        return result;
    }
    result = GetAudioPlayerBufferQueueInterface();
    if (SL_RESULT_SUCCESS != result) {
        return result;
    }
    result = RegisterPlayerCallback();
    if (SL_RESULT_SUCCESS != result) {
        return result;
    }
    result = GetAudioPlayerInterface();
    if (SL_RESULT_SUCCESS != result) {
        return result;
    }
    LOGI("leave init");
    return SL_RESULT_SUCCESS;
}

void SoundService::DestroyContext() {
    LOGI("enter SoundService::DestroyContext");
    DestroyObject(mAudioPlayerObject);
    FreePlayerBuffer();
    DestroyObject(mOutputMixObject);
    if (NULL != mDecoderController) {
        mDecoderController->destroy();
        delete mDecoderController;
        mDecoderController = NULL;
    }

    LOGI("leave SoundService::DestroyContext");
}









