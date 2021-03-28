package com.example.audiotracktest.audiotrack;

public class MusicDecoder implements Mp3Decoder {

    @Override
    public int init(String acPath) {
        return openFile(acPath);
    }

    @Override
    public int readSamples(short[] samples, int[] slientSizeArr) {
        return readSamples(samples, samples.length, slientSizeArr);
    }

    @Override
    public void destory() {
        closeFile();
    }

    private native int openFile(String acPath);

    private native int readSamples(short[] samples, int size, int[] slientSizeArr);

    private native void closeFile();
}
