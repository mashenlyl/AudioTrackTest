package com.example.audiotracktest.opensles;

public class SoundTrackController {
    public native boolean setAudioDataSource(String acPath);

    public native void play();

    public native void stop();
}
