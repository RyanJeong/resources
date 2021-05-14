package com.example.user.doorlockclient;

import android.content.Context;
import android.os.Bundle;
import android.text.method.ScrollingMovementMethod;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;

import static com.example.user.doorlockclient.R.layout.check_connection;

/**
 * Created by user on 2016-10-19.
 */
public class CheckConnection extends PrepareView {
    private ImageView icon;
    private SocketListener socketListener;
    private TextView textview;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(check_connection);

        processing = (TextView) findViewById(R.id.processing);
        processing.setText(pLog);
        processing.setMovementMethod(new ScrollingMovementMethod());
        sensing = (TextView) findViewById(R.id.sensing);
        sensing.setText(sLog);
        sensing.setMovementMethod(new ScrollingMovementMethod());
        icon = (ImageView) findViewById(R.id.connect_icon);
        icon.setOnTouchListener(clickListener);
        read();

        return;
    }

    private void read() {
        socketListener = new SocketListener(getApplicationContext(), handler);
        socketListener.start();

        return;
    }

    Button.OnTouchListener clickListener = new View.OnTouchListener() {

        @Override
        public boolean onTouch(View v, MotionEvent event) {
            ImageView image = (ImageView) v;

            if (v.getId() == R.id.connect_icon) {
                if (event.getAction() == MotionEvent.ACTION_DOWN) {
                    icon.setImageResource(R.drawable.connect_pressed);
                    setLog(fromClient, (-1), toProcessing, reConnect);
                    read();
                } else if (event.getAction() == MotionEvent.ACTION_UP) {
                    icon.setImageResource(R.drawable.connect_default);
                }
                icon.invalidate();
            }

            return true;
        }
    };
}
