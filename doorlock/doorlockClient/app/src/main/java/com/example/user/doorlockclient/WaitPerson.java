package com.example.user.doorlockclient;

import android.os.Bundle;
import android.text.method.ScrollingMovementMethod;
import android.widget.TextView;

import static com.example.user.doorlockclient.R.layout.wait_person;

/**
 * Created by user on 2016-10-15.
 */

public class WaitPerson extends PrepareView {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(wait_person);

        processing = (TextView) findViewById(R.id.processing);
        processing.setText(pLog);
        processing.setMovementMethod(new ScrollingMovementMethod());
        sensing = (TextView) findViewById(R.id.sensing);
        sensing.setText(sLog);
        sensing.setMovementMethod(new ScrollingMovementMethod());

        return;
    }
}
