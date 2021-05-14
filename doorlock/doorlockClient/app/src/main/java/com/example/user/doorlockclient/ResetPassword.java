package com.example.user.doorlockclient;

import android.os.Bundle;
import android.os.Handler;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import java.io.IOException;

import static com.example.user.doorlockclient.R.layout.reset_password;

/**
 * Created by user on 2016-10-15.
 */
public class ResetPassword extends PrepareView {
    private int count = 0, i = 0;
    private boolean check;
    private int[][] inputValue = new int[2][4];
    private TextView input1, input2, input3, input4;
    private TextView text;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(reset_password);

        text = (TextView) findViewById(R.id.text);
        input1 = (TextView) findViewById(R.id.input_code_1);
        input2 = (TextView) findViewById(R.id.input_code_2);
        input3 = (TextView) findViewById(R.id.input_code_3);
        input4 = (TextView) findViewById(R.id.input_code_4);
        findViewById(R.id.digit_0).setOnClickListener(buttonListener);
        findViewById(R.id.digit_1).setOnClickListener(buttonListener);
        findViewById(R.id.digit_2).setOnClickListener(buttonListener);
        findViewById(R.id.digit_3).setOnClickListener(buttonListener);
        findViewById(R.id.digit_4).setOnClickListener(buttonListener);
        findViewById(R.id.digit_5).setOnClickListener(buttonListener);
        findViewById(R.id.digit_6).setOnClickListener(buttonListener);
        findViewById(R.id.digit_7).setOnClickListener(buttonListener);
        findViewById(R.id.digit_8).setOnClickListener(buttonListener);
        findViewById(R.id.digit_9).setOnClickListener(buttonListener);
        findViewById(R.id.del).setOnClickListener(buttonListener);
        findViewById(R.id.back).setOnClickListener(buttonListener);

        check = false;
    }

    Button.OnClickListener buttonListener = new View.OnClickListener() {
        public void onClick(View v) {
            if (check) {
                check = !check;
                text.setText(insertPassword);
            }
            handler = new Handler();
            inputValue[i][count] = 0;
            switch (v.getId()) {
                case R.id.digit_9:
                    inputValue[i][count]++;
                case R.id.digit_8:
                    inputValue[i][count]++;
                case R.id.digit_7:
                    inputValue[i][count]++;
                case R.id.digit_6:
                    inputValue[i][count]++;
                case R.id.digit_5:
                    inputValue[i][count]++;
                case R.id.digit_4:
                    inputValue[i][count]++;
                case R.id.digit_3:
                    inputValue[i][count]++;
                case R.id.digit_2:
                    inputValue[i][count]++;
                case R.id.digit_1:
                    inputValue[i][count]++;
                case R.id.digit_0:
                    count++;
                    switch (count) {
                        case 4:
                            input4.setTextColor(inputColor);
                            count = 0;
                            handler.postDelayed(new Runnable() {
                                @Override
                                public void run() {
                                    input1.setTextColor(waitInputColor);
                                    input2.setTextColor(waitInputColor);
                                    input3.setTextColor(waitInputColor);
                                    input4.setTextColor(waitInputColor);
                                }
                            }, 256);
                            if (i == 0) {
                                text.setText(reinsertNewPassword);
                                i++;
                            } else {
                                i = 0;
                                if ((inputValue[0][0] == inputValue[1][0]) ? ((inputValue[0][1] == inputValue[1][1]) ? ((inputValue[0][2] == inputValue[1][2]) ? ((inputValue[0][3] == inputValue[1][3]) ? true : false) : false): false): false) {
                                    for (int j = 0; j < 4; j++) {
                                        setPassword(j, inputValue[0][j]);
                                    }
                                    showToast(resetPassword);
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
                                } else {
                                    text.setText(errorPassword);
                                }
                            }
                            break;
                        case 3:
                            input3.setTextColor(inputColor);
                            break;
                        case 2:
                            input2.setTextColor(inputColor);
                            break;
                        case 1:
                            input1.setTextColor(inputColor);
                            break;
                    }
                    break;
                case R.id.del:
                    if (count > 0) {
                        count--;
                    }
                    switch (count) {
                        case 0:
                            input1.setTextColor(waitInputColor);
                            break;
                        case 1:
                            input2.setTextColor(waitInputColor);
                            break;
                        case 2:
                            input3.setTextColor(waitInputColor);
                            break;
                    }
                    break;
                case R.id.back:
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
                    break;
            }

            return;
        }
    };
}
