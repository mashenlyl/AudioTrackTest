//
// Created by jetson on 2021/3/8.
//

#include "packet_pool.h"
#define LOG_TAG "liyaoliPacketPool"

PacketPool::PacketPool() {
}

PacketPool::~PacketPool() {
}

PacketPool* PacketPool::minstance = new PacketPool();
PacketPool* PacketPool::GetInstance() {
    return minstance;
}

void PacketPool::initDecoderAccompanyPacketQueue() {
    const char* name = "decoder accompany packet queue";
    macPacketQueue = new PacketQueue(name);
}

void PacketPool::abortDecoderAccompanyPacketQueue() {
    if (NULL != macPacketQueue) {
        macPacketQueue->abort();
    }
}

void PacketPool::destoryDecoderAccompanyPacketQueue() {
    if (NULL != macPacketQueue) {
        delete macPacketQueue;
        macPacketQueue = NULL;
    }
}

int PacketPool::getDecoderAccompanyPacket(AudioPacket **audioPacket, bool block) {
    int result = -1;
    if (NULL != macPacketQueue) {
        result = macPacketQueue->get(audioPacket, block);
    }
    return result;
}

int PacketPool::getDecoderAccompanyPacketQueueSize() {
    return macPacketQueue->size();
}

void PacketPool::clearDecoderAccompanyPacketQueue() {
    macPacketQueue->flush();
}

void PacketPool::pushDecoderAccompanyPacketQueue(AudioPacket *audioPacket) {
    macPacketQueue->put(audioPacket);
}
