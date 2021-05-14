package com.example.user.doorlockclient;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.KeyEvent;
import android.widget.TextView;
import android.widget.Toast;
import com.tsengvn.typekit.TypekitContextWrapper;

import java.text.DateFormat;
import java.util.Date;

/**
 * Created by user on 2016-10-17.
 */

public class PrepareView extends Activity {
    /* Log control, true : print Log, false : don't print Log */
    protected static final boolean isLog = true;

    protected static String currentDateTimeString = DateFormat.getDateTimeInstance().format(new Date());
    public static boolean isLock = false;
    public static boolean isConnect = false;
    public static Context mContext;
    protected static boolean isDoorOpened = true;
    protected static final int toProcessing = 0;
    protected static final int toSensing = 1;
    protected static final int fromUltrasonicSensor = 10;
    protected static final int fromCameraSensor = 11;
    protected static final int fromClient = 20;
    protected static final int fromServer = 21;
    protected static final int fromIOException = 22;
    protected static final int doorIsOpened = 30;
    protected static final int doorIsClosed = 31;
    protected static final int sensingStart = 32;
    protected static final int _socket = 33;
    protected static final int _connect = 34;
    protected static final int _connectSuccess = 35;
    protected static final int _connectError = 36;
    protected static final int _write = 37;
    protected static final int _writeSuccess = 38;
    protected static final int _writeError = 39;
    protected static final int _read = 40;
    protected static final int _readSuccess = 41;
    protected static final int _readError = 42;
    protected static final int _readEOF = 43;
    protected static final int _close = 44;
    protected static final int _closeError = 45;
    protected static final int _socketError = 46;
    protected static final int _setBuffer = 47;
    protected static final int _setBufferError = 48;
    protected static final int _initSocketListenerError = 49;
    protected static final int _setSocketManager = 50;
    protected static final int _setSocketManagerError = 51;
    protected static final int _serverIsClosed = 52;
    protected static final int _wifiOn = 53;
    protected static final int _wifiOff = 54;
    protected static String tag;
    protected static String wifiOn;
    protected static String wifiOff;
    protected static String checkWiFi;
    protected static String serverClosed;
    protected static String writeToServer;
    protected static String resetPattern;
    protected static String resetPassword;
    protected static String reConnect;
    protected static String sharedPreferences;
    protected static String lineSeparator;
    protected static String unidentified;
    protected static String bracketOpen;
    protected static String bracketClose;
    protected static String msgLock;
    protected static String msgUnlock;
    protected static String msgReady;
    protected static String msgKey01;
    protected static String msgKey02;
    protected static String msgKey03;
    protected static String msgKey04;
    protected static String password01;
    protected static String password02;
    protected static String password03;
    protected static String password04;
    protected static String pattern01;
    protected static String pattern02;
    protected static String pattern03;
    protected static String pattern04;
    protected static String call;
    protected static String ultrasonicSensor;
    protected static String cameraSensor;
    protected static String client;
    protected static String server;
    protected static String ioException;
    protected static String msgDoorIsOpened;
    protected static String msgDoorISClosed;
    protected static String msgSensingStart;
    protected static String msgSocket;
    protected static String msgConnect;
    protected static String msgConnectSuccess;
    protected static String msgConnectError;
    protected static String msgWrite;
    protected static String msgWriteSuccess;
    protected static String msgWriteError;
    protected static String msgRead;
    protected static String msgReadSuccess;
    protected static String msgReadError;
    protected static String msgReadEOF;
    protected static String msgClose;
    protected static String msgCloseError;
    protected static String msgSocketError;
    protected static String setBuffer;
    protected static String setBufferError;
    protected static String initSocketListenerError;
    protected static String setSocketManager;
    protected static String setSocketManagerError;
    protected static String serverIsClosed;
    protected static String errorPassword;
    protected static String insertPassword;
    protected static String insertNewPassword;
    protected static String reinsertNewPassword;
    protected static String insertPattern;
    protected static String insertNewPattern;

    protected static int inputColor;
    protected static int waitInputColor;
    protected static StringBuffer pLog = new StringBuffer();
    protected static StringBuffer sLog = new StringBuffer();
    protected static Handler handler;
    protected static TextView processing;
    protected static TextView sensing;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        tag = getResources().getString(R.string.doorlock);
        wifiOn = getResources().getString(R.string.wifiOn);
        wifiOff = getResources().getString(R.string.wifiOff);
        serverClosed = getResources().getString(R.string.server_closed);
        checkWiFi = getResources().getString(R.string.check_WiFi);
        writeToServer = getResources().getString(R.string.writeToServer);
        resetPattern = getResources().getString(R.string.reset_pattern);
        resetPassword = getResources().getString(R.string.reset_password);
        reConnect = getResources().getString(R.string.reConnect);
        sharedPreferences = getResources().getString(R.string.sharedPreferences);
        lineSeparator = getResources().getString(R.string.line_separator);
        unidentified = getResources().getString(R.string.unidentified);
        bracketOpen = getResources().getString(R.string.bracket_open);
        bracketClose = getResources().getString(R.string.bracket_close);
        msgLock = getResources().getString(R.string.lock);
        msgUnlock = getResources().getString(R.string.unlock);
        msgReady = getResources().getString(R.string.ready);
        msgKey01 = getResources().getString(R.string.key01);
        msgKey02 = getResources().getString(R.string.key02);
        msgKey03 = getResources().getString(R.string.key03);
        msgKey04 = getResources().getString(R.string.key04);
        password01 = getResources().getString(R.string.password01);
        password02 = getResources().getString(R.string.password02);
        password03 = getResources().getString(R.string.password03);
        password04 = getResources().getString(R.string.password04);
        pattern01 = getResources().getString(R.string.pattern01);
        pattern02 = getResources().getString(R.string.pattern02);
        pattern03 = getResources().getString(R.string.pattern03);
        pattern04 = getResources().getString(R.string.pattern04);
        call = getResources().getString(R.string.call);
        errorPassword = getResources().getString(R.string.password_error);
        insertPassword = getResources().getString(R.string.insert_password);
        insertPattern = getResources().getString(R.string.insert_pattern);
        insertNewPassword = getResources().getString(R.string.insert_new_password);
        insertNewPattern = getResources().getString(R.string.insert_new_pattern);
        reinsertNewPassword = getResources().getString(R.string.reinsert_new_password);
        ultrasonicSensor = getResources().getString(R.string.ultrasonicSensor);
        cameraSensor = getResources().getString(R.string.cameraSensor);
        client = getResources().getString(R.string.client);
        server = getResources().getString(R.string.server);
        ioException = getResources().getString(R.string.IOException);
        msgDoorIsOpened = getResources().getString(R.string.doorIsOpened);
        msgDoorISClosed = getResources().getString(R.string.doorIsClosed);
        msgSensingStart = getResources().getString(R.string.sensingStart);
        msgSocket = getResources().getString(R.string.socket);
        msgConnect = getResources().getString(R.string.connect);
        msgConnectSuccess = getResources().getString(R.string.connectSuccess);
        msgConnectError = getResources().getString(R.string.connectError);
        msgWrite = getResources().getString(R.string.write);
        msgWriteSuccess = getResources().getString(R.string.writeSuccess);
        msgWriteError = getResources().getString(R.string.writeError);
        msgRead = getResources().getString(R.string.read);
        msgReadSuccess = getResources().getString(R.string.readSuccess);
        msgReadError = getResources().getString(R.string.readError);
        msgReadEOF = getResources().getString(R.string.readEOF);
        msgClose = getResources().getString(R.string.close);
        msgCloseError = getResources().getString(R.string.closeError);
        msgSocketError = getResources().getString(R.string.socketError);
        setBuffer = getResources().getString(R.string.setBuffer);
        setBufferError = getResources().getString(R.string.setBufferError);
        initSocketListenerError = getResources().getString(R.string.initSocketListenerError);
        setSocketManager = getResources().getString(R.string.setSocketManager);
        setSocketManagerError = getResources().getString(R.string.setSocketManagerError);
        serverIsClosed = getResources().getString(R.string.serverIsClosed);
        inputColor = getResources().getColor(R.color.base);
        waitInputColor = getResources().getColor(R.color.waitValue);
        //waitInputColor = ContextCompat.getColor(getApplicationContext(), R.color.waitValue);

        return;
    }

    public void setLog(int source, int type, int destination, String msg) {
        (destination == toProcessing ? pLog : sLog).append(bracketOpen + currentDateTimeString + bracketClose);
        switch (source) {
            case fromUltrasonicSensor:
                (destination == toProcessing ? pLog : sLog).append(ultrasonicSensor);
                break;
            case fromCameraSensor:
                (destination == toProcessing ? pLog : sLog).append(cameraSensor);
                break;
            case fromClient:
                (destination == toProcessing ? pLog : sLog).append(client);
                break;
            case fromServer:
                (destination == toProcessing ? pLog : sLog).append(server);
                break;
            case fromIOException:
                (destination == toProcessing ? pLog : sLog).append(ioException);
                break;
            default:
                (destination == toProcessing ? pLog : sLog).append(unidentified);
                break;
        }
        switch (type) {
            case doorIsOpened:
                (destination == toProcessing ? pLog : sLog).append(msgDoorIsOpened);
                break;
            case doorIsClosed:
                (destination == toProcessing ? pLog : sLog).append(msgDoorISClosed);
                break;
            case sensingStart:
                (destination == toProcessing ? pLog : sLog).append(msgSensingStart);
                break;
            case _socket:
                (destination == toProcessing ? pLog : sLog).append(msgSocket);
                break;
            case _connect:
                (destination == toProcessing ? pLog : sLog).append(msgConnect);
                break;
            case _connectSuccess:
                (destination == toProcessing ? pLog : sLog).append(msgConnectSuccess);
                break;
            case _connectError:
                (destination == toProcessing ? pLog : sLog).append(msgConnectError);
                break;
            case _write:
                (destination == toProcessing ? pLog : sLog).append(msgWrite);
                break;
            case _writeSuccess:
                (destination == toProcessing ? pLog : sLog).append(msgWriteSuccess);
                break;
            case _writeError:
                (destination == toProcessing ? pLog : sLog).append(msgWriteError);
                break;
            case _read:
                (destination == toProcessing ? pLog : sLog).append(msgRead);
                break;
            case _readSuccess:
                (destination == toProcessing ? pLog : sLog).append(msgReadSuccess);
                break;
            case _readError:
                (destination == toProcessing ? pLog : sLog).append(msgReadError);
                break;
            case _readEOF:
                (destination == toProcessing ? pLog : sLog).append(msgReadEOF);
                break;
            case _close:
                (destination == toProcessing ? pLog : sLog).append(msgClose);
                break;
            case _closeError:
                (destination == toProcessing ? pLog : sLog).append(msgCloseError);
                break;
            case _socketError:
                (destination == toProcessing ? pLog : sLog).append(msgSocketError);
                break;
            case _setBuffer:
                (destination == toProcessing ? pLog : sLog).append(setBuffer);
                break;
            case _setBufferError:
                (destination == toProcessing ? pLog : sLog).append(setBufferError);
                break;
            case _initSocketListenerError:
                (destination == toProcessing ? pLog : sLog).append(initSocketListenerError);
                break;
            case _setSocketManager:
                (destination == toProcessing ? pLog : sLog).append(setSocketManager);
                break;
            case _setSocketManagerError:
                (destination == toProcessing ? pLog : sLog).append(setSocketManagerError);
                break;
            case _serverIsClosed:
                (destination == toProcessing ? pLog : sLog).append(serverIsClosed);
                break;
            case _wifiOn:
                (destination == toProcessing ? pLog : sLog).append(wifiOn);
                break;
            case _wifiOff:
                (destination == toProcessing ? pLog : sLog).append(wifiOff);
                break;
            default:
                (destination == toProcessing ? pLog : sLog).append(msg);
                break;
        }
        (destination == toProcessing ? pLog : sLog).append(System.getProperty(lineSeparator));
        textViewHandler.sendEmptyMessage(destination);

        return;
    }

    Handler textViewHandler = new Handler() {

        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            switch (msg.what) {
                case toProcessing:
                    processing.setText(pLog);
                    break;
                case toSensing:
                    sensing.setText(sLog);
                    break;
                default:
                    break;
            }

            return;
        }
    };

    @Override
    protected void attachBaseContext(Context newBase) {
        super.attachBaseContext(TypekitContextWrapper.wrap(newBase));

        return;
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        switch (keyCode) {
            case KeyEvent.KEYCODE_BACK:

                return true;
        }

        return super.onKeyDown(keyCode, event);
    }

    protected void showToast(String string) {
        showLog(tag, "[PrepareView], call showToast()");
        Toast.makeText (this, string, Toast.LENGTH_SHORT).show();

        return;
    }

    public void callActivityFromThread(Context context, Class c) {
        Intent intent = new Intent(context, c);

        showLog(tag, "[PrepareView], call callActivityFromThread()");
        intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        context.startActivity(intent);
        finish();
    }

    protected void callActivity(Class c) {
        Intent intent = new Intent(this, c);

        showLog(tag, "[PrepareView], call callActivity()");
        startActivity(intent);
        finish();

        return;
    }

    /*
        init password : 0112
     */
    protected int getPassword(int i) {
        SharedPreferences pw = getSharedPreferences(sharedPreferences, 0);

        showLog(tag, "[PrepareView], call getPassword()");
        switch (i) {
            case 0:

                return pw.getInt(password01, 0);
            case 1:

                return pw.getInt(password02, 1);
            case 2:

                return pw.getInt(password03, 1);
            case 3:

                return pw.getInt(password04, 2);
            default:

                return 0;
        }
    }

    protected void setPassword(int i, int password) {
        SharedPreferences pref = getSharedPreferences(sharedPreferences, 0);
        SharedPreferences.Editor editor = pref.edit();

        showLog(tag, "[PrepareView], call setPassword()");
        switch (i) {
            case 0:
                editor.putInt(password01, password);
                break;
            case 1:
                editor.putInt(password02, password);
                break;
            case 2:
                editor.putInt(password03, password);
                break;
            case 3:
                editor.putInt(password04, password);
                break;
            default:
                break;
        }
        editor.commit();

        return;
    }

    /*
        init pattern : L L R L
        Left    Right
        true    false
     */

    protected boolean getPattern(int i) {
        SharedPreferences pw = getSharedPreferences(sharedPreferences, 0);

        showLog(tag, "[PrepareView], call getPattern()");
        switch (i) {
            case 0:

                return pw.getBoolean(pattern01, true);
            case 1:

                return pw.getBoolean(pattern02, true);
            case 2:

                return pw.getBoolean(pattern03, false);
            case 3:

                return pw.getBoolean(pattern04, true);
            default:

                return true;
        }
    }

    protected void setPattern(int i, boolean direction) {
        SharedPreferences pref = getSharedPreferences(sharedPreferences, 0);
        SharedPreferences.Editor editor = pref.edit();

        showLog(tag, "[PrepareView], call setPassword()");
        switch (i) {
            case 0:
                editor.putBoolean(pattern01, direction);
                break;
            case 1:
                editor.putBoolean(pattern02, direction);
                break;
            case 2:
                editor.putBoolean(pattern03, direction);
                break;
            case 3:
                editor.putBoolean(pattern04, direction);
                break;
            default:
                break;
        }
        editor.commit();

        return;
    }

    public void showLog(String tag, String msg) {
        if (isLog) {
            Log.i(tag, msg);
        }
    }
}
