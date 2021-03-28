package com.example.audiotracktest.audiotrack;

public interface Mp3Decoder {
    public int init(String acPath);

    public void destory();

    public int readSamples(short[] samples, int[] slientSizeArr);
}
