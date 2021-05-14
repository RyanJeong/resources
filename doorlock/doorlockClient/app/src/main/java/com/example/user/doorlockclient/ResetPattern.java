package com.example.user.doorlockclient;

import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;

import java.io.IOException;

import static com.example.user.doorlockclient.R.layout.reset_pattern;

/**
 * Created by user on 2016-10-15.
 */

/*
    Left    Right
    true    false
 */

public class ResetPattern extends PrepareView {
    private ImageView pattern[] = new ImageView[4];
    private ImageView okIcon, backIcon;
    private boolean direction[] = new boolean[4];

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(reset_pattern);

        pattern[0] = (ImageView) findViewById(R.id.pattern01);
        pattern[1] = (ImageView) findViewById(R.id.pattern02);
        pattern[2] = (ImageView) findViewById(R.id.pattern03);
        pattern[3] = (ImageView) findViewById(R.id.pattern04);
        okIcon = (ImageView) findViewById(R.id.ok);
        okIcon.setOnTouchListener(clickListener);
        backIcon = (ImageView) findViewById(R.id.back);
        backIcon.setOnTouchListener(clickListener);
        for (int i = 0; i < 4; i++) {
            direction[i] = getPattern(i);
            if (direction[i]) {
                pattern[i].setImageResource(R.drawable.left_default);
            } else {
                pattern[i].setImageResource(R.drawable.right_default);
            }
            pattern[i].setOnTouchListener(clickListener);
            pattern[i].invalidate();
        }
    }

    Button.OnTouchListener clickListener = new View.OnTouchListener() {

        @Override
        public boolean onTouch(View v, MotionEvent event) {
            ImageView image = (ImageView) v;
            int i, j;

            i = 0;
            switch (v.getId()) {
                case R.id.pattern04:
                    i++;
                case R.id.pattern03:
                    i++;
                case R.id.pattern02:
                    i++;
                case R.id.pattern01:
                    if (direction[i]) {
                        if (event.getAction() == MotionEvent.ACTION_DOWN) {
                            pattern[i].setImageResource(R.drawable.left_pressed);
                        } else if (event.getAction() == MotionEvent.ACTION_UP) {
                            direction[i] = !direction[i];
                            pattern[i].setImageResource(R.drawable.right_default);
                        }
                    } else {
                        if (event.getAction() == MotionEvent.ACTION_DOWN) {
                            pattern[i].setImageResource(R.drawable.right_pressed);
                        } else if (event.getAction() == MotionEvent.ACTION_UP) {
                            direction[i] = !direction[i];
                            pattern[i].setImageResource(R.drawable.left_default);
                        }
                    }
                    showLog(tag, String.valueOf(i) + " " + String.valueOf(direction[i]));
                    pattern[i].invalidate();
                    break;
                case R.id.ok:
                    if (event.getAction() == MotionEvent.ACTION_DOWN) {
                        okIcon.setImageResource(R.drawable.ok_pressed);
                    } else if (event.getAction() == MotionEvent.ACTION_UP) {
                        okIcon.setImageResource(R.drawable.ok_default);
                        for (j = 0; j < 4; j++) {
                            setPattern(j, direction[j]);
                        }
                        showToast(resetPattern);
                        try {
                            setLog(fromClient, (-1), toProcessing, writeToServer + msgUnlock);
                            SocketManager.writeMsg(msgUnlock);
                            isLock = false;
                            if (isConnect) {
                                callActivity((isDoorOpened == true) ? AgreementPattern.class : WaitPerson.class);
                            }
                        } catch (IOException e) {
                            setLog(fromClient, _writeError, toProcessing, null);
                        }
                    }
                    okIcon.invalidate();
                    break;
                case R.id.back:
                    if (event.getAction() == MotionEvent.ACTION_DOWN) {
                        backIcon.setImageResource(R.drawable.back_pressed);
                    } else if (event.getAction() == MotionEvent.ACTION_UP) {
                        backIcon.setImageResource(R.drawable.back_default);
                        try {
                            setLog(fromClient, (-1), toProcessing, writeToServer + msgUnlock);
                            SocketManager.writeMsg(msgUnlock);
                            isLock = false;
                            if (isConnect) {
                                callActivity((isDoorOpened == true) ? AgreementPattern.class : WaitPerson.class);
                            }
                        } catch (IOException e) {
                            setLog(fromClient, _writeError, toProcessing, null);
                        }
                    }
                    backIcon.invalidate();
                    break;
            }

            return true;
        }
    };
}
