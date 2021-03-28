#ifndef ACCOMPANY_DECODER_CONTROLLER_H
#define ACCOMPANY_DECODER_CONTROLLER_H

#include <unistd.h>
#include "packet_pool.h"
#include "accompany_decoder.h"

#define CHANNEL_PER_FRAME 2
#define BITS_PER_CHANNEL 16
#define BITS_PER_BYTE 8
#define QUEUE_SIZE_MAX_THRESHOLD 25
#define QUEUE_SIZE_MIN_THRESHOLD 20

class AccompanyDecoderController {
protected:
    AccompanyDecoder* macDecoder;
    PacketPool* mpacketPool;
    pthread_t songDecoderThread;
    static void* startDecoderThread(void* ptr);
    virtual void initDecoderThread();
    virtual void decodeSongPacket();
    virtual void destroyDecoderThread();

public:
    bool isRunning;
    pthread_mutex_t mLock;
    pthread_cond_t mCondition;

    AccompanyDecoderController();
    ~AccompanyDecoderController();

    virtual int init(const char* acPath);
    virtual int readSamples(short* samples, int size, int* slientSizeArr);
    virtual void destroy();
};





#endif