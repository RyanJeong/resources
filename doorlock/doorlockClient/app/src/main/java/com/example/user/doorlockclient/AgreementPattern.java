package com.example.user.doorlockclient;

import android.os.Bundle;
import android.text.method.ScrollingMovementMethod;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;

import java.io.IOException;

import static com.example.user.doorlockclient.R.layout.agreement_pattern;

/**
 * Created by user on 2016-10-15.
 */

public class AgreementPattern extends PrepareView {
    private ImageView showLog, resetPassword, resetPattern;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(agreement_pattern);

        processing = (TextView) findViewById(R.id.processing);
        processing.setText(pLog);
        processing.setMovementMethod(new ScrollingMovementMethod());
        sensing = (TextView) findViewById(R.id.sensing);
        sensing.setText(sLog);
        sensing.setMovementMethod(new ScrollingMovementMethod());
        showLog = (ImageView) findViewById(R.id.show_log);
        showLog.setOnTouchListener(clickListener);
        resetPassword = (ImageView) findViewById(R.id.reset_password);
        resetPassword.setOnTouchListener(clickListener);
        resetPattern = (ImageView) findViewById(R.id.reset_pattern);
        resetPattern.setOnTouchListener(clickListener);
        isDoorOpened = true;

        return;
    }

    Button.OnTouchListener clickListener = new View.OnTouchListener() {

        @Override
        public boolean onTouch(View v, MotionEvent event) {
            ImageView image = (ImageView) v;

            switch (v.getId()) {
                case R.id.show_log:
                    if (event.getAction() == MotionEvent.ACTION_DOWN) {
                        showLog.setImageResource(R.drawable.show_log_pressed);
                        try {
                            setLog(fromClient, (-1), toProcessing, writeToServer + msgLock);
                            SocketManager.writeMsg(msgLock);
                            isLock = true;
                        } catch (IOException e) {
                            setLog(fromClient, _writeError, toProcessing, null);
                        }
                        setLog(fromClient, (-1), toProcessing, call + String.valueOf(ShowLog.class));
                        callActivity(ShowLog.class);
                    } else if (event.getAction() == MotionEvent.ACTION_UP) {
                        showLog.setImageResource(R.drawable.show_log_default);
                    }
                    showLog.invalidate();
                    break;
                case R.id.reset_password:
                    if (event.getAction() == MotionEvent.ACTION_DOWN) {
                        resetPassword.setImageResource(R.drawable.reset_password_pressed);
                        try {
                            setLog(fromClient, (-1), toProcessing, writeToServer + msgLock);
                            SocketManager.writeMsg(msgLock);
                            isLock = true;
                        } catch (IOException e) {
                            setLog(fromClient, _writeError, toProcessing, null);
                        }
                        setLog(fromClient, (-1), toProcessing, call + String.valueOf(ResetPassword.class));
                        callActivity(ResetPassword.class);
                    } else if (event.getAction() == MotionEvent.ACTION_UP) {
                        resetPassword.setImageResource(R.drawable.reset_password_default);
                    }
                    resetPassword.invalidate();
                    break;
                case R.id.reset_pattern:
                    if (event.getAction() == MotionEvent.ACTION_DOWN) {
                        resetPattern.setImageResource(R.drawable.reset_pattern_pressed);
                        try {
                            setLog(fromClient, (-1), toProcessing, writeToServer + msgLock);
                            SocketManager.writeMsg(msgLock);
                            isLock = true;
                        } catch (IOException e) {
                            setLog(fromClient, _writeError, toProcessing, null);
                        }
                        setLog(fromClient, (-1), toProcessing, call + String.valueOf(ResetPattern.class));
                        callActivity(ResetPattern.class);
                    } else if (event.getAction() == MotionEvent.ACTION_UP) {
                        resetPattern.setImageResource(R.drawable.reset_pattern_default);
                    }
                    resetPattern.invalidate();
                    break;
                default:
                    break;
            }

            return true;
        }
    };
}
