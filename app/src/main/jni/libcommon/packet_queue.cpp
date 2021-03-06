//
// Created by jetson on 2021/3/8.
//

#include "packet_queue.h"
#define LOG_TAG "lylPacketQueue"

PacketQueue::PacketQueue() {
    init();
}

PacketQueue::PacketQueue(const char* queueNameParam) {
    init();
    mQueueName = queueNameParam;
}

void PacketQueue::init() {
    LOGI("enter PacketQueue init");
    int initLockCode = pthread_mutex_init(&mLock, NULL);
    int initConditionCode = pthread_cond_init(&mCondition, NULL);
    mNbPackets = 0;
    mFirst = NULL;
    mLast = NULL;
    mAbortRequest = false;
}

PacketQueue::~PacketQueue() {
    LOGI("enter ~PacketQueue");
    flush();
    pthread_mutex_destroy(&mLock);
    pthread_cond_destroy(&mCondition);
}

int PacketQueue::size() {
    LOGI("enter PacketQueue size %d", mNbPackets);
    pthread_mutex_lock(&mLock);
    int size = mNbPackets;
    pthread_mutex_unlock(&mLock);
    return size;
}

void PacketQueue::flush() {
    LOGI("%s flush .... and this time the queue size is %d", mQueueName, size());
    AudioPacketList *pkt, *pkt1;

    AudioPacket* audioPacket;
    pthread_mutex_lock(&mLock);

    for(pkt = mFirst; pkt != NULL; pkt = pkt1) {
        pkt1 = pkt->next;
        audioPacket = pkt->pkt;
        if (NULL != audioPacket) {
            delete audioPacket;
        }
        delete pkt;
        pkt = NULL;
    }
    mLast = NULL;
    mFirst = NULL;
    mNbPackets = 0;

    pthread_mutex_unlock(&mLock);
}

int PacketQueue::put(AudioPacket *audioPacket) {
    LOGI("enter PacketQueue put...");
    if (mAbortRequest) {
        delete audioPacket;
        return -1;
    }
    AudioPacketList *pkt1 = new AudioPacketList();
    if (!pkt1)
        return -1;
    pkt1->pkt = audioPacket;
    pkt1->next = NULL;

    int getLockCode = pthread_mutex_lock(&mLock);
    if (mLast == NULL) {
        mFirst = pkt1;
    } else {
        mLast->next = pkt1;
    }

    mLast = pkt1;
    mNbPackets++;

    pthread_cond_signal(&mCondition);
    pthread_mutex_unlock(&mLock);
    return 0;
}

int PacketQueue::get(AudioPacket **audioPacket, bool block) {
    LOGI("enter PacketQueue get...");
    AudioPacketList* pkt1;
    int ret;

    int getLockCode = pthread_mutex_lock(&mLock);
    for (;;) {
        if (mAbortRequest) {
            ret = -1;
            break;
        }
        pkt1 = mFirst;
        if (pkt1) {
            mFirst = pkt1->next;
            if (!mFirst)
                mLast = NULL;
            mNbPackets--;
            *audioPacket = pkt1->pkt;
            delete pkt1;
            pkt1 = NULL;
            ret = 1;
            break;
        } else if (!block) {
            ret = 0;
            break;
        } else {
            pthread_cond_wait(&mCondition, &mLock);
        }
    }

    pthread_mutex_unlock(&mLock);
    return ret;
}

void PacketQueue::abort() {
    LOGI("enter PacketQueue abort...");
    pthread_mutex_lock(&mLock);
    mAbortRequest = true;
    pthread_cond_signal(&mCondition);
    pthread_mutex_unlock(&mLock);
}










