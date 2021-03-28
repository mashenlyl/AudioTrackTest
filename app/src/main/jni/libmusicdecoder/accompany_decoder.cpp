//
// Created by jetson on 2021/3/2.
//

#include "accompany_decoder.h"

#define MAX_AUDIO_FRAME_SIZE 192000
#define LOG_TAG "liyaoliAcDecoder"

AccompanyDecoder::AccompanyDecoder() {
    macFilePath = NULL;
}

AccompanyDecoder::~AccompanyDecoder() {
    if (NULL != macFilePath) {
        delete [] macFilePath;
        macFilePath = NULL;
    }
}

int AccompanyDecoder::init(const char *fileString) {
    LOGI("enter AccompanyDecoder::init");
    mavFormatContext = NULL;
    mstream_index = -1;
    mavCodecContext = NULL;
    mswrContext = NULL;
    AVCodec * avCodec = NULL;
    mindex = 0;

    mpAudioFrame = av_frame_alloc();
    if (mpAudioFrame == NULL) {
        LOGI("av_frame_alloc mpAudioFrame fail...");
        return -1;
    } else {
        LOGI("av_frame_alloc mpAudioFrame success...");
    }
    maudioBuffer = (uint8_t *)av_malloc(MAX_AUDIO_FRAME_SIZE*2);
    if (maudioBuffer == NULL) {
        LOGI("av_malloc audiobuffer fail...");
        return -1;
    } else {
        LOGI("av_malloc audiobuffer success...");
    }
    // 注册解码器
    avcodec_register_all();
    av_register_all();
    mavFormatContext = avformat_alloc_context();
    LOGI("open ac file %s...", fileString);

    // 打开文件，并解析文件，然后填充avformat
    int result = avformat_open_input(&mavFormatContext, fileString, NULL, NULL);
    if (result != 0) {
        LOGI("can't open file %s result %s", fileString, av_err2str(result));
        return -1;
    } else {
        LOGI("open file %s success and result is %s", fileString, av_err2str(result));
    }

    // 检查文件中的流信息
    result = avformat_find_stream_info(mavFormatContext, NULL);
    if (result < 0) {
        LOGI("fail avformat avformat_find_stream_info %s result %s", fileString,
             av_err2str(result));
        return -1;
    } else {
        LOGI("avformat_find_stream_info success result is %s", fileString, av_err2str(result));
    }
    mstream_index = av_find_best_stream(mavFormatContext, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    LOGI("stream_index is %d", mstream_index);
    if (mstream_index == -1) {
        LOGI("no audio stream");
        return -1;
    }
    mavCodecContext = mavFormatContext->streams[mstream_index]->codec;
    LOGI("avCodecContext->codec_id is %d AV_CODEC_ID_AAC is %d", mavCodecContext->codec_id,
         AV_CODEC_ID_AAC);
    avCodec = avcodec_find_decoder(mavCodecContext->codec_id);
    if (avCodec == NULL) {
        LOGI("Unsupported codec");
        return -1;
    }
    result = avcodec_open2(mavCodecContext, avCodec, NULL);
    if (result < 0) {
        LOGI("avcodec_open2 fail avformat_find_stream_info result is %s", av_err2str(result));
        return -1;
    } else {
        LOGI("avcodec_open2 sucess avformat_find_stream_info result is %s", av_err2str(result));
    }
    if (!(mavCodecContext->sample_fmt == AV_SAMPLE_FMT_S16)) {
        LOGI("because of audio Codec Is Not Supported so we will init swresampler...");
        mswrContext = swr_alloc();
        mswrContext = swr_alloc_set_opts(mswrContext,
                                         av_get_default_channel_layout(OUT_PUT_CHANNELS),
                                         AV_SAMPLE_FMT_S16, 44100,
                                         av_get_default_channel_layout(mavCodecContext->channels),
                                         mavCodecContext->sample_fmt, mavCodecContext->sample_rate,
                                         0, NULL);
        if (!mswrContext || swr_init(mswrContext)) {
            if (mswrContext)
                swr_free(&mswrContext);
            avcodec_close(mavCodecContext);
            LOGI("init resampler failed...");
            return -1;
        }
    }
    LOGI("liyaoli channels is %d sampleRate is %d", mavCodecContext->channels,
         mavCodecContext->sample_rate);

    return 0;
}

AudioPacket* AccompanyDecoder::decodePacket() {
    int ret = 1;
    int gotframe = 0;
    AVSampleFormat outSampleFmt = AV_SAMPLE_FMT_S16;
    AudioPacket* samplePacket = NULL;

    av_init_packet(&mpacket);

    while (av_read_frame(mavFormatContext, &mpacket) >= 0) {
        //LOGI("liyaoli av_read_frame start");
        if (mpacket.stream_index == mstream_index) {
            //LOGI("liyaoli av_read_frame mstream_index");
            if (avcodec_decode_audio4(mavCodecContext, mpAudioFrame, &gotframe, &mpacket) < 0) {
                LOGI("decode audio error, skip packet");
                return NULL;
            }
            //LOGI("liyaoli av_read_frame avcodec_decode_audio4");
            if (gotframe) {
                //LOGI("liyaoli av_read_frame gotframe");
                int outBufferSize=mpAudioFrame->nb_samples * OUT_PUT_CHANNELS;
                int numFrames = 0;
                // 重新采样
                if (mswrContext) {
                    numFrames = swr_convert(mswrContext, &maudioBuffer,
                                            mpAudioFrame->nb_samples,
                            (const uint8_t **)mpAudioFrame->data,
                            mpAudioFrame->nb_samples);
                    if (numFrames < 0) {
                        LOGI("fail resample audio");
                        return NULL;
                    }
                    LOGI("mindex:%5d\t pts:%lld\t packet size:%d out_buffer_size: %d\n",mindex,mpacket.pts,mpacket.size, outBufferSize);
                    // copy to AudioPacket
                    short * samples = new short[outBufferSize];
                    memcpy(samples, maudioBuffer, outBufferSize*2);
                    samplePacket = new AudioPacket();
                    if (samplePacket == NULL) {
                        return NULL;
                    }
                    samplePacket->mbuffer = samples;
                    samplePacket->msize = outBufferSize;
                    mindex++;
                    break;
                }
            }
        }
    }
    //LOGI("liyaoli end");
    av_packet_unref(&mpacket);

    return samplePacket;
}


void AccompanyDecoder::destroy() {
    if (maudioBuffer != NULL) {
        av_free(maudioBuffer);
        maudioBuffer = NULL;
    }
    if (NULL != mswrContext) {
        swr_free(&mswrContext);
        mswrContext = NULL;
    }
    if (NULL != mpAudioFrame) {
        av_free(mpAudioFrame);
        mpAudioFrame = NULL;
    }
    if (NULL != mavCodecContext) {
        avcodec_close(mavCodecContext);
        mavCodecContext = NULL;
    }
    if (NULL != mavFormatContext) {
        LOGI("leave LiveReceiver::destory");
        avformat_close_input(&mavFormatContext);
        mavFormatContext = NULL;
    }
    av_free(&mpacket);
}