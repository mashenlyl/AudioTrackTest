package com.example.audiotracktest.audiotrack;

import android.media.AudioAttributes;
import android.media.AudioFormat;
import android.media.AudioTrack;
import android.util.Log;

public class NativeMp3Player {

    public static final String TAG = "NativeMp3Player";
    private int mAudioStreamType = -1;
    private Mp3Decoder mDecoder;
    private AudioTrack mAudioTrack;
    private Thread mPlayerThread;
    private static int DECODE_BUFFER_SIZE = 192000;

    private volatile boolean isStop = false;
    private volatile boolean isPlaying = false;

    public void setAudioStreamType(int audioStreamType) {
        this.mAudioStreamType = audioStreamType;
    }

    public int setDataSource(String path) {
        mDecoder = new MusicDecoder();
        return mDecoder.init(path);
    }

    public void prepare() {
        initPlayState();
        initAudioTrack();
        startPlayerThread();
    }

    private void initPlayState() {
        isPlaying = false;
        isStop = false;
    }

    private void startPlayerThread() {
        mPlayerThread = new Thread(new PlayerThread(), "NativeMp3PlayerThread");
        mPlayerThread.start();
    }

    private void initAudioTrack() {
        int buffersize = AudioTrack.getMinBufferSize(44100, AudioFormat.CHANNEL_OUT_STEREO, AudioFormat.ENCODING_PCM_16BIT);
        mAudioTrack = new AudioTrack.Builder()
                .setAudioAttributes(new AudioAttributes.Builder()
                        .setUsage(AudioAttributes.USAGE_MEDIA)
                        .setContentType(AudioAttributes.CONTENT_TYPE_MUSIC)
                        .build())
                .setAudioFormat(new AudioFormat.Builder()
                        .setEncoding(AudioFormat.ENCODING_PCM_16BIT)
                        .setSampleRate(44100)
                        .setChannelMask(AudioFormat.CHANNEL_OUT_STEREO)
                        .build())
                .setBufferSizeInBytes(buffersize)
                .build();
    }

    public void start() {
        synchronized (NativeMp3Player.class) {
            try {
                if (null != mAudioTrack) {
                    mAudioTrack.play();
                }
            } catch (Throwable t) {

            }
            isPlaying = true;
        }
    }

    public void stop() {
        if (!isStop && null != mAudioTrack) {
            if (null != mAudioTrack && mAudioTrack.getState() != AudioTrack.STATE_INITIALIZED) {
                try {
                    mAudioTrack.stop();
                } catch (Throwable t) {
                    t.printStackTrace();
                }
            }
            isPlaying = true;
            isStop = true;
            try {
                Log.i(TAG, "join decodeMp3Thread...");
                if (null != mPlayerThread) {
                    mPlayerThread.join();
                    mPlayerThread = null;
                }
                Log.e(TAG, "decodeMp3Thread ended....");
            } catch (Throwable t) {
                t.printStackTrace();
            }
            closeAudioTrack();
            destroy();
        }
    }

    public void destroy() {
        mPlayerThread = null;
        mAudioTrack = null;
    }

    private void closeAudioTrack() {
        try {
            if (null != mAudioTrack && mAudioTrack.getState() != AudioTrack.STATE_INITIALIZED) {
                mAudioTrack.release();
            }
        } catch (Throwable t) {
            t.printStackTrace();
        }
    }

    class PlayerThread implements Runnable {
        private short[] samples;

        @Override
        public void run() {
            int sample_count = 0;
            boolean isPlayTemp = isPlaying = false;
            try {
                samples = new short[DECODE_BUFFER_SIZE];
                int[] extraSlientSampleSize = new int[1];

                while (!isStop) {
                    extraSlientSampleSize[0] = 0;
                    sample_count = mDecoder.readSamples(samples, extraSlientSampleSize);

                    if (sample_count == -2) {
                        try {
                            Thread.sleep(10);
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }
                        continue;
                    }
                    if (sample_count < 0) {
                        break;
                    }

                    if (null != mAudioTrack && mAudioTrack.getState() != AudioTrack.STATE_UNINITIALIZED) {
                        mAudioTrack.write(samples, 0, sample_count);
                    }

                    while (true) {
                        synchronized (NativeMp3Player.class) {
                            isPlayTemp = isPlaying;
                        }
                        if (isPlayTemp)
                            break;
                        else
                            Thread.yield();
                    }
                }
                mDecoder.destory();
            } catch (Error e) {
                e.printStackTrace();
            }
            samples = null;
        }
    }

}
