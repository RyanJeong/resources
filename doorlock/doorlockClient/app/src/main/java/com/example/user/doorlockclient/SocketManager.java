package com.example.user.doorlockclient;

import android.util.Log;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketAddress;

/**
 * Created by user on 2016-10-24.
 */
public class SocketManager {
    public final static String HOST = "192.168.0.100";
    public final static int PORT = 6000;
    public final static int TIMEOUT = 6000;
    private static boolean isClose = true;
    private static boolean isLog;
    private static String tag = PrepareView.tag;
    private static Socket socket;

    public static Socket socket() {
        if ((socket == null) || !isClose) {
            showLog(tag, "[SocketManager], call socket()");
            socket = new Socket();
            isClose = true;
        }
        if (!socket.isConnected()) {
            SocketAddress socketAddress = new InetSocketAddress(HOST, PORT);
            try {
                showLog(tag, "[SocketManager], call connect()");
                socket.connect(socketAddress, TIMEOUT);
            } catch (IOException e) {
                try {
                    close();
                } catch (IOException e1) {
                    showLog(tag, "[SocketManager], connect() error(try-catch(try-catch))" + e1.getMessage());
                }
            }
        }

        return socket;
    }

    public static void close() throws IOException {
        if (socket != null) {
            showLog(tag, "[SocketManager], call close()");
            socket.close();
            isClose = false;
        }

        return;
    }

    public static void writeMsg(String msg) throws IOException {
        showLog(tag, "[SocketManager], call writeMsg()");
        if (PrepareView.isConnect) {
            socket().getOutputStream().write((msg + '\0').getBytes());
        }

        return;
    }

    public static void showLog(String tag, String msg) {
        isLog = PrepareView.isLog;
        if (isLog) {
            Log.i(tag, msg);
        }

        return;
    }
}
