//
// Created by jetson on 2021/3/13.
//

#ifndef AUDIOTRACKTEST_SOUND_SERVICE_H
#define AUDIOTRACKTEST_SOUND_SERVICE_H

#include "opensl_es_context.h"
#include "../libmusicdecoder/accompany_decoder_controller.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

#define PLAYING_STATE_STOPPED (0x00000001)
#define PLAYING_STATE_PLAYING (0x00000002)

class SoundService {
private:
    SoundService();
    static SoundService* mInstance;
    int mPlayingState;
    AccompanyDecoderController* mDecoderController;

    SLEngineItf mEngine;
    SLObjectItf mOutputMixObject;
    SLObjectItf mAudioPlayerObject;
    SLAndroidSimpleBufferQueueItf mAudioPlayerBufferQueue;
    SLPlayItf mAudioPlayerPlay;

    int mBufferNums;
    int mCurrentFrame;
    int mPacketBufferSize;
    unsigned char* mBuffer;
    size_t mBufferSize;
    short* mTarget;

    SLresult RealizeObject(SLObjectItf object) {
        return (*object)->Realize(object, SL_BOOLEAN_FALSE); // No async, blocking call
    };

    void DestroyObject(SLObjectItf& object) {
        if (0 != object)
            (*object)->Destroy(object);
        object = 0;
    };

    SLresult CreateOutputMix() {
        return (*mEngine)->CreateOutputMix(mEngine, &mOutputMixObject, 0, 0, 0);
    };

    void FreePlayerBuffer() {
        if (NULL != mBuffer) {
            delete [] mBuffer;
            mBuffer = NULL;
        }
        if (NULL != mTarget) {
            delete [] mTarget;
            mTarget = NULL;
        }
    };

    void InitPlayerBuffer() {
        mBuffer = new unsigned char[mBufferSize];
        mTarget = new short[mBufferSize / 2];
    }

    SLresult CreateBufferQueueAudioPlayer() {
        SLDataLocator_AndroidSimpleBufferQueue dataSourceLocator = {
                SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, // locator type
                1  // buffer count
        };
        SLDataFormat_PCM dataSourceFormat = {
                SL_DATAFORMAT_PCM,  // format type
                2,  // channel count
                SL_SAMPLINGRATE_44_1, // samples per second in millihertz
                SL_PCMSAMPLEFORMAT_FIXED_16, // bits per sample
                SL_PCMSAMPLEFORMAT_FIXED_16, // container size
                SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT, // channel mask
                SL_BYTEORDER_LITTLEENDIAN // endianness
        };
        SLDataSource dataSource = {
                &dataSourceLocator,
                &dataSourceFormat
        };
        SLDataLocator_OutputMix dataSinkLocator = {
                SL_DATALOCATOR_OUTPUTMIX,
                mOutputMixObject
        };
        SLDataSink dataSink = {
                &dataSinkLocator,
                0
        };
        SLInterfaceID interfaceIds[] = { SL_IID_BUFFERQUEUE };
        SLboolean requiredInterfaces[] =  { SL_BOOLEAN_TRUE };

        return (*mEngine)->CreateAudioPlayer(mEngine, &mAudioPlayerObject,
                &dataSource, &dataSink, ARRAY_LEN(interfaceIds),
                interfaceIds, requiredInterfaces);
    };

    SLresult GetAudioPlayerBufferQueueInterface() {
        return (*mAudioPlayerObject)->GetInterface(mAudioPlayerObject, SL_IID_BUFFERQUEUE,
                &mAudioPlayerBufferQueue);
    };

    static void PlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context) {
        SoundService* service = (SoundService*) context;
        service->producePacket();
    };

    SLresult RegisterPlayerCallback();

    SLresult GetAudioPlayerInterface() {
        return (*mAudioPlayerObject)->GetInterface(mAudioPlayerObject, SL_IID_PLAY,
                &mAudioPlayerPlay);
    };

    SLresult SetAudioPlayerStatePlaying() {
        return (*mAudioPlayerPlay)->SetPlayState(mAudioPlayerPlay, SL_PLAYSTATE_PLAYING);
    };

    SLresult SetAudioPlayerStatePaused() {
        return (*mAudioPlayerPlay)->SetPlayState(mAudioPlayerPlay, SL_PLAYSTATE_PAUSED);
    };

    SLresult SetAudioPlayerStateStoped() {
        return (*mAudioPlayerPlay)->SetPlayState(mAudioPlayerPlay, SL_PLAYSTATE_STOPPED);
    };

public:
    static SoundService* GetInstance();
    virtual ~SoundService();
    void initSongDecoder(const char* acPath);
    SLresult initSoundTrack();
    SLresult play();
    SLresult stop();
    void producePacket();
    void DestroyContext();
};


#endif //AUDIOTRACKTEST_SOUND_SERVICE_H












