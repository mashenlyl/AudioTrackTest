//
// Created by jetson on 2021/3/2.
//

#include "accompany_decoder_controller.h"
#define LOG_TAG "liyaoliacDecoderController"

AccompanyDecoderController::AccompanyDecoderController() {
    macDecoder = NULL;
}

AccompanyDecoderController::~AccompanyDecoderController() {

}

void* AccompanyDecoderController::startDecoderThread(void* ptr) {
    LOGI("enter AccompanyDecoderController::startDecoderThread");
    AccompanyDecoderController* decoderController = (AccompanyDecoderController *) ptr;
    int getLockCode = pthread_mutex_lock(&decoderController->mLock);
    while (decoderController->isRunning) {
        decoderController->decodeSongPacket();
        if (decoderController->mpacketPool->getDecoderAccompanyPacketQueueSize() > QUEUE_SIZE_MAX_THRESHOLD) {
            pthread_cond_wait(&decoderController->mCondition, &decoderController->mLock);
        }
    }
}

int AccompanyDecoderController::init(const char *acPath) {
    LOGI("AccompanyDecoderController::Init");
    macDecoder = new AccompanyDecoder();
    if (macDecoder->init(acPath) < 0) {
        LOGI("macDecoder->init fail...");
        return -1;
    } else {
        LOGI("macDecoder->init success...");
    }

    mpacketPool = PacketPool::GetInstance();
    mpacketPool->initDecoderAccompanyPacketQueue();
    initDecoderThread();
    return 0;
}

void AccompanyDecoderController::initDecoderThread() {
    LOGI("enter AccompanyDecoderController::initDecoderThread");
    isRunning = true;
    pthread_mutex_init(&mLock, NULL);
    pthread_cond_init(&mCondition, NULL);
    pthread_create(&songDecoderThread, NULL, startDecoderThread, this);
}

void AccompanyDecoderController::decodeSongPacket() {
    LOGI("AccompanyDecoderController::decodeSongPacket");
    AudioPacket* acPacket = macDecoder->decodePacket();
    if (acPacket != NULL) {
        acPacket->maction = AudioPacket::AUDIO_PACKET_ACTION_PLAY;
        mpacketPool->pushDecoderAccompanyPacketQueue(acPacket);
    }
}

void AccompanyDecoderController::destroyDecoderThread() {
    LOGI("AccompanyDecoderController::destroyDecoderThread");
    isRunning = false;
    void* status;
    int getLockCode = pthread_mutex_lock(&mLock);
    pthread_cond_signal(&mCondition);
    pthread_mutex_unlock(&mLock);
    pthread_join(songDecoderThread, &status);
    pthread_mutex_destroy(&mLock);
    pthread_cond_destroy(&mCondition);
}

int AccompanyDecoderController::readSamples(short* samples, int size, int* slientSizeArr) {
    LOGI("AccompanyDecoderController::readSamples");
    int result = -1;
    AudioPacket* acPacket = NULL;
    mpacketPool->getDecoderAccompanyPacket(&acPacket, true);
    if (NULL != acPacket) {
        int samplePacketSize = acPacket->msize;
        if (samplePacketSize != -1 && samplePacketSize <= size) {
            memcpy(samples, acPacket->mbuffer, samplePacketSize * 2);
            delete acPacket;
            result = samplePacketSize;
        }
    } else {
        result = -2;
    }
    if (mpacketPool->getDecoderAccompanyPacketQueueSize() < QUEUE_SIZE_MIN_THRESHOLD) {
        int getLockCode = pthread_mutex_lock(&mLock);
        if (result != -1) {
            pthread_cond_signal(&mCondition);
        }
        pthread_mutex_unlock(&mLock);
    }
    return result;
}

void AccompanyDecoderController::destroy() {
    LOGI("AccompanyDecoderController::Destroy");
    destroyDecoderThread();
    mpacketPool->abortDecoderAccompanyPacketQueue();
    mpacketPool->destoryDecoderAccompanyPacketQueue();
    if (NULL != macDecoder) {
        macDecoder->destroy();
        delete macDecoder;
        macDecoder = NULL;
    }
}








