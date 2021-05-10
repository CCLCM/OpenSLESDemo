package com.zhidao.open_sl;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

    }


    public native void playPcm(String url);

    public void playMusic(View view) {
        String path = Environment.getExternalStorageDirectory().getPath()+"/beijing16bit.pcm";
        if (new File(path).exists()) {
            Log.d("chencl",path);
        }
        playPcm(path);
    }
}