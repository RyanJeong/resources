package com.example.user.doorlockclient;

import android.os.Bundle;
import android.os.Handler;

/*
    1. OpenCv
    2. ShowLog(Camera Capture and Store)
    3. Response(Communication Check!)
 */

public class MainActivity extends PrepareView {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mContext = getApplicationContext();

        handler = new Handler();
        handler.postDelayed(new Runnable() {

            @Override
            public void run() {
                callActivity(InsertPassword.class);
            }
        }, 4000);

        return;
    }
}