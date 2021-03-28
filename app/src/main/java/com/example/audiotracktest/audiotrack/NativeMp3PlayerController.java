package com.example.audiotracktest.audiotrack;

import android.media.AudioManager;

import android.os.Handler;
import android.util.Log;

public class NativeMp3PlayerController {

    public static final String TAG = "NativeMp3PlayerController";
    public NativeMp3Player mediaPlayer;
    //private Handler mHandler;
    private boolean isPlaying = false;

    public NativeMp3PlayerController() {
        try {
            if (mediaPlayer == null) {
                mediaPlayer = new NativeMp3Player();
                mediaPlayer.setAudioStreamType(AudioManager.STREAM_MUSIC);
            }
        } catch (Exception e) {

        }
    }

    /*public void setHandler(Handler mTimeHandler) {
        this.mHandler = mTimeHandler;
    }*/

    public int setAudioDataSource(String path) {
        int result = mediaPlayer.setDataSource(path);
        if (result == 0) {
            mediaPlayer.prepare();
        } else {
            Log.e(TAG, "NativeMp3PlayerController setAudioDataSource fail");
        }
        return result;
    }

    public void start() {
        isPlaying = true;
        try {
            if (mediaPlayer != null) {
                mediaPlayer.start();
            }
        } catch (IllegalStateException e) {
            e.printStackTrace();
        }
    }

    public void stop() {
        try {
            if (mediaPlayer != null) {
                mediaPlayer.stop();
            }
        } catch (IllegalStateException e) {
            e.printStackTrace();
        }
        isPlaying = false;
    }

    public NativeMp3Player getMediaPlayer() {
        return mediaPlayer;
    }

}
