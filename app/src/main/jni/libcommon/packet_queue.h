//
// Created by jetson on 2021/3/8.
//

#ifndef AUDIOTRACKTEST_PACKET_QUEUE_H
#define AUDIOTRACKTEST_PACKET_QUEUE_H

#include <pthread.h>
#include "CommonTools.h"

typedef struct AudioPacket {

    static const int AUDIO_PACKET_ACTION_PLAY = 0;
    static const int AUDIO_PACKET_ACTION_PAUSE = 100;
    static const int AUDIO_PACKET_ACTION_SEEK = 101;

    short * mbuffer;
    int msize;
    int maction;

    AudioPacket() {
        mbuffer = NULL;
        msize = 0;
        maction = -1;
    }

    ~AudioPacket() {
        if (NULL != mbuffer) {
            delete [] mbuffer;
            mbuffer = NULL;
        }
    }
} AudioPacket;

typedef struct AudioPacketList {
    AudioPacket* pkt;
    struct AudioPacketList *next;
    AudioPacketList() {
        pkt = NULL;
        next = NULL;
    }
} AudioPacketList;

class PacketQueue {
public:
    PacketQueue();
    PacketQueue(const char* queueNameParam);
    ~PacketQueue();

    void init();
    void flush();
    int put(AudioPacket* audioPacket);

    int get(AudioPacket** audioPacket, bool block);
    int size();
    void abort();

private:
    AudioPacketList * mFirst;
    AudioPacketList * mLast;
    int mNbPackets;
    bool mAbortRequest;
    pthread_mutex_t mLock;
    pthread_cond_t mCondition;
    const char* mQueueName;
};


#endif //AUDIOTRACKTEST_PACKET_QUEUE_H
