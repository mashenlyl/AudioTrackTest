//
// Created by jetson on 2021/3/8.
//

#ifndef AUDIOTRACKTEST_PACKET_POOL_H
#define AUDIOTRACKTEST_PACKET_POOL_H

#include "packet_queue.h"


class PacketPool {
private:
    PacketPool();
    static PacketPool* minstance;
    PacketQueue* macPacketQueue;

public:
    static PacketPool* GetInstance();
    virtual ~PacketPool();

    virtual void initDecoderAccompanyPacketQueue();
    virtual void abortDecoderAccompanyPacketQueue();
    virtual void destoryDecoderAccompanyPacketQueue();
    virtual int getDecoderAccompanyPacket(AudioPacket **audioPacket, bool block);
    virtual void pushDecoderAccompanyPacketQueue(AudioPacket* audioPacket);
    virtual void clearDecoderAccompanyPacketQueue();
    virtual int getDecoderAccompanyPacketQueueSize();
};


#endif //AUDIOTRACKTEST_PACKET_POOL_H
