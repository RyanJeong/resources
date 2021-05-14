package com.example.user.doorlockclient;

import android.os.Bundle;
import android.os.Handler;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import static com.example.user.doorlockclient.R.layout.insert_password;

/**
 * Created by user on 2016-10-15.
 */
public class InsertPassword extends PrepareView {
    private int count = 0;
    private boolean check;
    private int[] inputValue = new int[4];
    private int[] password = new int[4];
    private TextView input1, input2, input3, input4;
    private TextView text;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(insert_password);

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
        for (int i = 0; i < 4; i++) {
            password[i] = getPassword(i);
        }

        return;
    }

    Button.OnClickListener buttonListener = new View.OnClickListener() {
        public void onClick(View v) {
            handler = new Handler();
            inputValue[count] = 0;
            if (check) {
                check = !check;
                text.setText(insertPassword);
            }
            switch (v.getId()) {
                case R.id.digit_9:
                    inputValue[count]++;
                case R.id.digit_8:
                    inputValue[count]++;
                case R.id.digit_7:
                    inputValue[count]++;
                case R.id.digit_6:
                    inputValue[count]++;
                case R.id.digit_5:
                    inputValue[count]++;
                case R.id.digit_4:
                    inputValue[count]++;
                case R.id.digit_3:
                    inputValue[count]++;
                case R.id.digit_2:
                    inputValue[count]++;
                case R.id.digit_1:
                    inputValue[count]++;
                case R.id.digit_0:
                    count++;
                    switch (count) {
                        case 4:
                            count = 0;
                            input4.setTextColor(inputColor);
                            handler.postDelayed(new Runnable() {

                                @Override
                                public void run() {
                                    input1.setTextColor(waitInputColor);
                                    input2.setTextColor(waitInputColor);
                                    input3.setTextColor(waitInputColor);
                                    input4.setTextColor(waitInputColor);
                                }
                            }, 256);
                            if ((password[0] == inputValue[0]) ? ((password[1] == inputValue[1]) ? ((password[2] == inputValue[2]) ? ((password[3] == inputValue[3]) ? true : false) : false): false): false) {
                                callActivity(CheckConnection.class);
                            } else {
                                check = !check;
                                text.setText(errorPassword);
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
                    finish();
                    break;
            }

            return;
        }
    };
}
