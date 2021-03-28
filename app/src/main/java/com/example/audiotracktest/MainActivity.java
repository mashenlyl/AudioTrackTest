package com.example.audiotracktest;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.widget.Button;

import com.example.audiotracktest.audiotrack.NativeMp3PlayerController;
import com.example.audiotracktest.opensles.SoundTrackController;

public class MainActivity extends AppCompatActivity {

    static {
        System.loadLibrary("songstudio");
    }

    private static String TAG = "lylMainActivity";

    private Button audioTrackPlayBtn;
    private Button audioTrackStopBtn;
    private Button openslesPlayBtn;
    private Button openslesStopBtn;

    private static String playFilePath = "/storage/emulated/0/Android/data/com.example.audiotracktest/cache/123.mp3";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Log.e(TAG, getExternalCacheDir().getPath());
        findView();
        bindListener();
        openSLPlayerController = new SoundTrackController();
        openSLPlayerController.setAudioDataSource(playFilePath);
    }

    private void findView() {
        audioTrackPlayBtn = (Button) findViewById(R.id.play_audiotrack_btn);
        audioTrackStopBtn = (Button) findViewById(R.id.stop_audiotrack_btn);
        openslesPlayBtn = (Button) findViewById(R.id.play_opensl_btn);
        openslesStopBtn = (Button) findViewById(R.id.stop_opensl_btn);
    }

    private void bindListener() {
        audioTrackPlayBtn.setOnClickListener(audioTrackPlayBtnListener);
        audioTrackStopBtn.setOnClickListener(audioTrackStopBtnListener);
        openslesPlayBtn.setOnClickListener(openSLESPlayBtnListener);
        openslesStopBtn.setOnClickListener(openSLESStopBtnListener);
    }

    private NativeMp3PlayerController audioTrackPlayerController;
    View.OnClickListener audioTrackPlayBtnListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            Log.i(TAG, "Click AudioTrack Play Btn");
            audioTrackPlayerController = new NativeMp3PlayerController();
            //audioTrackPlayerController.setHandler(mhandler);
            audioTrackPlayerController.setAudioDataSource(playFilePath);
            audioTrackPlayerController.start();
        }
    };

    View.OnClickListener audioTrackStopBtnListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            Log.i(TAG, "Click AudioTrack Stop Btn");
            if (null != audioTrackPlayerController) {
                audioTrackPlayerController.stop();
                audioTrackPlayerController = null;
            }
        }
    };

    private SoundTrackController openSLPlayerController;
    private Thread mOpenSLThread = new Thread(new OpenSLThread(), "OpenSLThread");
    View.OnClickListener openSLESPlayBtnListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            Log.i(TAG, "Click OpenSL ES Play Btn");
            mOpenSLThread.start();
        }
    };

    View.OnClickListener openSLESStopBtnListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            Log.i(TAG, "Click OpenSL ES Stop Btn");
            if (null != openSLPlayerController) {
                openSLPlayerController.stop();
                openSLPlayerController = null;
            }
        }
    };

    class OpenSLThread implements Runnable {
        @Override
        public void run() {
            openSLPlayerController.play();
        }
    }

    /*private Handler mhandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            // 以回调的方式类计算进度条
            int _time = Math.max(msg.arg1, 0) / 1000;
            int total_time = Math.max(msg.arg2, 0) / 1000;
            float ratio = (float) _time / (float) total_time;
            Log.i(TAG, "Play Progress: " + ratio);
        }
    };*/
}
