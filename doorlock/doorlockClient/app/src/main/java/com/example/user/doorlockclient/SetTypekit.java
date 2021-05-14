package com.example.user.doorlockclient;

import android.app.Application;

import com.tsengvn.typekit.Typekit;

/**
 * Created by user on 2016-10-17.
 */
public class SetTypekit extends Application {

    @Override
    public void onCreate() {
        super.onCreate();

        Typekit.getInstance()
                .addCustom1(Typekit.createFromAsset(this, "nanum_pen.ttf"));

        return;
    }
}
