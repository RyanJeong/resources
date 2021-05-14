package com.example.user.doorlockclient;

import android.content.Context;
import android.os.Handler;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;

/**
 * Created by user on 2016-11-01.
 */
public class SocketListener extends Thread {
    private InputStream inputStream;
    private BufferedReader bufferedReader;
    private Handler mHandler ;
    private Context context;
    private static final String goToWaitPerson = "C";
    private static final String goToInsertPattern = "S";
    private static final String goToAgreementPattern = "O";
    private PrepareView prepareView = new PrepareView();

    public SocketListener(Context context, Handler handler) {
        prepareView.showLog(prepareView.tag, "[SocketListener], set SocketListener thread.");
        this.mHandler = handler;
        this.context = context;

        return;
    }

    @Override
    public void run() {
        String readMsg;

        super.run();
        prepareView.showLog(prepareView.tag, "[SocketListener], start SocketListener thread.");
        try {
            prepareView.setLog(prepareView.fromClient, prepareView._socket, prepareView.toProcessing, null);
            prepareView.setLog(prepareView.fromClient, prepareView._connect, prepareView.toProcessing, null);
            inputStream = SocketManager.socket().getInputStream();
            prepareView.setLog(prepareView.fromClient, prepareView._read, prepareView.toProcessing, null);
            bufferedReader = new BufferedReader(new InputStreamReader(inputStream));
            prepareView.showLog(prepareView.tag, "[SocketListener], start read from Server.");
            PrepareView.isConnect = true;
            while ((readMsg = bufferedReader.readLine()) != null) {
                prepareView.showLog(prepareView.tag, "[SocketListener] readMsg: " + readMsg);
                switch (readMsg) {
                    case goToWaitPerson:
                        if (PrepareView.isLock) {
                            prepareView.showLog(prepareView.tag, "[SocketListener] isLock");
                            PrepareView.isDoorOpened = false;
                        } else {
                            prepareView.setLog(prepareView.fromClient, (-1), prepareView.toProcessing, prepareView.call + String.valueOf(WaitPerson.class));
                            prepareView.callActivityFromThread(this.context, WaitPerson.class);
                        }
                        break;
                    case goToInsertPattern:
                        prepareView.setLog(prepareView.fromClient, (-1), prepareView.toProcessing, prepareView.call + String.valueOf(InsertPattern.class));
                        prepareView.callActivityFromThread(this.context, InsertPattern.class);
                        break;
                    case goToAgreementPattern:
                        if (PrepareView.isLock) {
                            prepareView.showLog(prepareView.tag, "[SocketListener] isLock");
                            PrepareView.isDoorOpened = true;
                        } else {
                            prepareView.setLog(prepareView.fromClient, (-1), prepareView.toProcessing, prepareView.call + String.valueOf(AgreementPattern.class));
                            prepareView.callActivityFromThread(this.context, AgreementPattern.class);
                        }
                        break;
                    default:
                        prepareView.setLog(prepareView.fromUltrasonicSensor, (-1), prepareView.toSensing, readMsg);
                        break;
                }
            }
            PrepareView.isConnect = false;
            while (PrepareView.isLock) {
                ;
            }
            prepareView.setLog(prepareView.fromClient, prepareView._readError, prepareView.toProcessing, null);
            prepareView.callActivityFromThread(this.context, CheckConnection.class);
        } catch (IOException e) {
            prepareView.setLog(prepareView.fromIOException, prepareView._readError, prepareView.toProcessing, null);
            prepareView.showLog(prepareView.tag, "[SocketListener], read error(try-catch), " + e.getMessage());
            if (PrepareView.isConnect) {
                prepareView.callActivityFromThread(this.context, CheckConnection.class);
                PrepareView.isConnect = false;
            }
        } finally {
            try {
                prepareView.setLog(prepareView.fromClient, prepareView._close, prepareView.toProcessing, null);
                PrepareView.isLock = false;
                SocketManager.close();
            } catch (IOException e) {
                prepareView.setLog(prepareView.fromIOException, prepareView._closeError, prepareView.toProcessing, null);
                prepareView.showLog(prepareView.tag, "[SocketListener], read error(try-catch-finally(try-catch)), " + e.getMessage());
            }
        }

        return;
    }
}
