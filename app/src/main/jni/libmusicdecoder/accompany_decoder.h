//
// Created by jetson on 2021/3/2.
//

#ifndef FFMPEGDECODER_ACCOMPANY_DECODER_H
#define FFMPEGDECODER_ACCOMPANY_DECODER_H

#include "CommonTools.h"
#include "packet_queue.h"

extern "C" {
    #include "../3rdparty/ffmpeg/include/libavcodec/avcodec.h"
    #include "../3rdparty/ffmpeg/include/libavcodec/packet.h"
    #include "../3rdparty/ffmpeg/include/libavformat/avformat.h"
    #include "../3rdparty/ffmpeg/include/libavutil/avutil.h"
    #include "../3rdparty/ffmpeg/include/libavutil/mem.h"
    #include "../3rdparty/ffmpeg/include/libavutil/samplefmt.h"
    #include "../3rdparty/ffmpeg/include/libavutil/common.h"
    #include "../3rdparty/ffmpeg/include/libavutil/channel_layout.h"
    #include "../3rdparty/ffmpeg/include/libavutil/opt.h"
    #include "../3rdparty/ffmpeg/include/libavutil/frame.h"
    #include "../3rdparty/ffmpeg/include/libavutil/imgutils.h"
    #include "../3rdparty/ffmpeg/include/libavutil/mathematics.h"
    #include "../3rdparty/ffmpeg/include/libswscale/swscale.h"
    #include "../3rdparty/ffmpeg/include/libswresample/swresample.h"
};

#define OUT_PUT_CHANNELS 2

class AccompanyDecoder {
private:
    AVFormatContext* mavFormatContext;
    int mstream_index;
    AVCodecContext* mavCodecContext;
    SwrContext *mswrContext;
    uint8_t *maudioBuffer;
    AVFrame* mpAudioFrame;
    AVPacket mpacket;
    char* macFilePath;
    int mindex;

public:
    AccompanyDecoder();
    virtual ~AccompanyDecoder();

    virtual int init(const char* mp3Path);
    virtual AudioPacket* decodePacket();
    virtual void destroy();
};



#endif //FFMPEGDECODER_ACCOMPANY_DECODER_H
