package com.example.jung.ppg;

import android.app.ProgressDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.support.v4.app.FragmentManager;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.ImageView;
import android.widget.Toast;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Calendar;
import java.util.Set;
import java.util.UUID;

public class MainActivity extends AppCompatActivity {
    private final static float      MAX_VALUE = (float) 255.0;
    private final static int        DELAY = 50;
    private final static int        DEVICES_DIALOG = 1;
    private final static int        ERROR_DIALOG = 2;
    private final static int        BASE = 64;
    private final static int        LEVEL01 = 8;
    private final static int        LEVEL02 = 7;
    private final static int        LEVEL03 = 6;
    private final static int        LEVEL04 = 5;
    private final static int        LEVEL05 = 4;
    private final static int        LEVEL06 = 3;
    private final static int        LEVEL07 = 2;
    private final static int        LEVEL08 = 1;
    private final static int        NUM_OF_DATA = (BASE * LEVEL01);
    private static float            datas[] = new float[NUM_OF_DATA];
    private static int              endPoint, currentPoint;
    public static Context           mContext;
    public static AppCompatActivity activity;
    ImageView                       btnConnect, btnStream, btnScroll, BtnZoomIn, BtnZoomOut;
    IntentFilter                    checkBluetoothStateFilter, catchDiscoveredDevicesFilter, checkACLStateFilter;
    static BluetoothAdapter         bluetoothAdapter;
    BluetoothDevice                 bluetoothDevice;
    BluetoothSocket                 bluetoothSocket;
    OutputStream                    outputStream;
    InputStream                     inputStream;
    public static boolean           isStreamed, isScrolled, isActivated, isPaired, maxZoomIn, maxZoomOut, isWorking, isDrawing;
    int                             countZoomLevel;
    int                             surfaceViewWidth, surfaceViewHeight;
    float                           interval, data;
    volatile boolean                stopReading;
    Thread                          readThread;
    ProgressDialog                  progress;
    SurfaceView                     surfaceView;
    SurfaceHolder                   surfaceHolder;
    DrawThread                      drawThread;


    public final BroadcastReceiver checkBluetoothState = new BroadcastReceiver() {

        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();

            if (action.equals(BluetoothAdapter.ACTION_STATE_CHANGED)) {
                final int state = intent.getIntExtra(BluetoothAdapter.EXTRA_STATE, BluetoothAdapter.ERROR);

                switch (state) {
                    case BluetoothAdapter.STATE_TURNING_OFF:

                        break;
                    case BluetoothAdapter.STATE_OFF:
                        prepareBluetoothOff();

                        break;
                    case BluetoothAdapter.STATE_TURNING_ON:

                        break;
                    case BluetoothAdapter.STATE_ON:
                        prepareBluetoothOn();

                        break;
                    case BluetoothAdapter.STATE_CONNECTED:

                        break;
                    case BluetoothAdapter.STATE_CONNECTING:

                        break;
                    case BluetoothAdapter.STATE_DISCONNECTED:

                        break;
                    case BluetoothAdapter.STATE_DISCONNECTING:

                        break;
                }
                btnConnect.invalidate();
                btnStream.invalidate();
                btnScroll.invalidate();
                BtnZoomIn.invalidate();
                BtnZoomOut.invalidate();
            }

            return;
        }
    };

    public final BroadcastReceiver catchDiscoveredDevices = new BroadcastReceiver() {

        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();

            if (action.equals(BluetoothAdapter.ACTION_SCAN_MODE_CHANGED)) {
                int mode = intent.getIntExtra(BluetoothAdapter.EXTRA_SCAN_MODE, BluetoothAdapter.ERROR);

                switch (mode) {
                    case BluetoothAdapter.SCAN_MODE_CONNECTABLE_DISCOVERABLE:

                        break;
                    case BluetoothAdapter.SCAN_MODE_CONNECTABLE:

                        break;
                    case BluetoothAdapter.SCAN_MODE_NONE:

                        break;
                }
            }

            return;
        }
    };

    public final BroadcastReceiver checkACLState = new BroadcastReceiver() {

        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();

            switch (action) {
                case BluetoothDevice.ACTION_ACL_CONNECTED:
                    prepareConnection();
                    showToast(getString(R.string.bluetoothConnected));
                    if (progress.isShowing()) {
                        progress.dismiss();
                    }

                    break;
                case BluetoothDevice.ACTION_ACL_DISCONNECTED:
                    prepareDisconnection();
                    showToast(getString(R.string.bluetoothDisconnected));
                    if (progress.isShowing()) {
                        progress.dismiss();
                    }

                    break;
                case BluetoothDevice.ACTION_BOND_STATE_CHANGED:

                    break;
                case BluetoothDevice.ACTION_ACL_DISCONNECT_REQUESTED:

                    break;
                case BluetoothDevice.ACTION_CLASS_CHANGED:

                    break;
                case BluetoothDevice.ACTION_FOUND:

                    break;
                case BluetoothDevice.ACTION_NAME_CHANGED:

                    break;
                case BluetoothDevice.ACTION_PAIRING_REQUEST:

                    break;
                case BluetoothDevice.ACTION_UUID:

                    break;
                case BluetoothDevice.EXTRA_BOND_STATE:

                    break;
                case BluetoothDevice.EXTRA_CLASS:

                    break;
                case BluetoothDevice.EXTRA_DEVICE:

                    break;
                case BluetoothDevice.EXTRA_NAME:

                    break;
                case BluetoothDevice.EXTRA_PAIRING_KEY:

                    break;
                case BluetoothDevice.EXTRA_PAIRING_VARIANT:

                    break;
                case BluetoothDevice.EXTRA_PREVIOUS_BOND_STATE:

                    break;
                case BluetoothDevice.EXTRA_RSSI:

                    break;
                case BluetoothDevice.EXTRA_UUID:

                    break;
            }
            btnConnect.invalidate();
            btnStream.invalidate();
            btnScroll.invalidate();
            BtnZoomIn.invalidate();
            BtnZoomOut.invalidate();

            return;
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        surfaceView = (SurfaceView) findViewById(R.id.surfaceView);
        surfaceHolder = surfaceView.getHolder();
        surfaceHolder.addCallback(surfaceListener);
        mContext = this;
        activity = this;
        bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        checkBluetoothStateFilter = new IntentFilter(BluetoothAdapter.ACTION_STATE_CHANGED);
        registerReceiver(checkBluetoothState, checkBluetoothStateFilter);
        catchDiscoveredDevicesFilter = new IntentFilter();
        catchDiscoveredDevicesFilter.addAction(BluetoothAdapter.ACTION_DISCOVERY_STARTED);
        catchDiscoveredDevicesFilter.addAction(BluetoothAdapter.ACTION_DISCOVERY_FINISHED);
        catchDiscoveredDevicesFilter.addAction(BluetoothAdapter.ACTION_SCAN_MODE_CHANGED);
        registerReceiver(catchDiscoveredDevices, catchDiscoveredDevicesFilter);
        checkACLStateFilter = new IntentFilter();
        checkACLStateFilter.addAction(BluetoothDevice.ACTION_ACL_CONNECTED);
        checkACLStateFilter.addAction(BluetoothDevice.ACTION_ACL_DISCONNECTED);
        registerReceiver(checkACLState, checkACLStateFilter);
        btnConnect  = (ImageView) findViewById(R.id.btn_connect);
        btnStream   = (ImageView) findViewById(R.id.btn_stream);
        btnScroll   = (ImageView) findViewById(R.id.btn_scroll);
        BtnZoomIn   = (ImageView) findViewById(R.id.btn_zoom_in);
        BtnZoomOut  = (ImageView) findViewById(R.id.btn_zoom_out);
        if (bluetoothAdapter == null) {
            showToast(getString(R.string.bluetoothNotAvailable));
            finish();

            return;
        } else if (!bluetoothAdapter.isEnabled()) {
            prepareBluetoothOff();
        } else if (bluetoothAdapter.isEnabled() && !bluetoothAdapter.isDiscovering()) {
            prepareBluetoothOn();
        } else if (bluetoothAdapter.isDiscovering()) {
            prepareConnection();
        }
        btnConnect.setOnTouchListener(new View.OnTouchListener() {

            @Override
            public boolean onTouch(View v, MotionEvent event) {
                if (isActivated) {
                    switch (event.getAction()) {
                        case MotionEvent.ACTION_DOWN:
                            if (!isPaired) {
                                btnConnect.setImageResource(R.drawable.connect_pushed);
                                DeviceDialog();
                            } else {
                                btnConnect.setImageResource(R.drawable.disconnect_pushed);
                                doDisconnect();
                                isPaired = false;
                                drawThread.isExit = true;
                                while (true) {
                                    try {
                                        drawThread.join();
                                        break;
                                    } catch (InterruptedException e) {

                                    }
                                }
                            }

                            break;
                        case MotionEvent.ACTION_UP:
                            btnConnect.setImageResource(R.drawable.connect_normal);
                            break;
                    }
                }

                return false;
            }
        });
        btnStream.setOnTouchListener(new View.OnTouchListener() {

            @Override
            public boolean onTouch(View v, MotionEvent event) {
                if (isPaired) {
                    switch (event.getAction()) {
                        case MotionEvent.ACTION_DOWN:
                            if (!isStreamed) {
                                btnStream.setImageResource(R.drawable.start_streaming_pushed);
                            } else {
                                btnStream.setImageResource(R.drawable.stop_streaming_pushed);
                            }

                            break;
                        case MotionEvent.ACTION_UP:
                            if (!isStreamed) {
                                if (drawThread != null) {
                                    drawThread = null;
                                    drawThread = new DrawThread(surfaceHolder);
                                    drawThread.start();
                                }
                                btnStream.setImageResource(R.drawable.stop_streaming_normal);
                            } else {
                                drawThread.isExit = true;
                                while (true) {
                                    try {
                                        drawThread.join();
                                        break;
                                    } catch (InterruptedException e) {

                                    }
                                }
                                btnStream.setImageResource(R.drawable.start_streaming_normal);
                            }
                            isStreamed = !isStreamed;
                            break;
                    }
                }

                return false;
            }
        });
        btnScroll.setOnTouchListener(new View.OnTouchListener() {

            @Override
            public boolean onTouch(View v, MotionEvent event) {
                if (isPaired) {
                    switch (event.getAction()) {
                        case MotionEvent.ACTION_DOWN:
                            if (!isScrolled) {
                                btnScroll.setImageResource(R.drawable.realtime_pushed);
                            } else {
                                btnScroll.setImageResource(R.drawable.fixed_frame_pushed);
                            }

                            break;
                        case MotionEvent.ACTION_UP:
                            if (!isScrolled) {
                                btnScroll.setImageResource(R.drawable.fixed_frame_normal);
                            } else {
                                btnScroll.setImageResource(R.drawable.realtime_normal);
                            }
                            isScrolled = !isScrolled;
                            break;
                    }
                }

                return false;
            }
        });
        BtnZoomIn.setOnTouchListener(new View.OnTouchListener() {

            @Override
            public boolean onTouch(View v, MotionEvent event) {
                if (isPaired) {
                    switch (event.getAction()) {
                        case MotionEvent.ACTION_DOWN:
                            if (!maxZoomIn) {
                                BtnZoomIn.setImageResource(R.drawable.zoom_in_pushed);
                                countZoomLevel++;
                                endPoint -= BASE;
                                if (maxZoomOut) {
                                    maxZoomOut = false;
                                    BtnZoomOut.setImageResource(R.drawable.zoom_out_normal);
                                }
                                if (countZoomLevel >= LEVEL01) {
                                    maxZoomIn = true;
                                }
                            }

                            break;
                        case MotionEvent.ACTION_UP:
                            if (!maxZoomIn) {
                                BtnZoomIn.setImageResource(R.drawable.zoom_in_normal);
                            } else {
                                BtnZoomIn.setImageResource(R.drawable.zoom_in_untouchable);
                            }
                            if (currentPoint >= endPoint) {
                                currentPoint = 0;
                            }

                            break;
                    }
                }
                interval = ((float) surfaceViewWidth / (float) endPoint);

                return false;
            }
        });
        BtnZoomOut.setOnTouchListener(new View.OnTouchListener() {

            @Override
            public boolean onTouch(View v, MotionEvent event) {
                if (isPaired) {
                    switch (event.getAction()) {
                        case MotionEvent.ACTION_DOWN:
                            if (!maxZoomOut) {
                                BtnZoomOut.setImageResource(R.drawable.zoom_out_pushed);
                                countZoomLevel--;
                                endPoint += BASE;
                                if (maxZoomIn) {
                                    maxZoomIn = false;
                                    BtnZoomIn.setImageResource(R.drawable.zoom_in_normal);
                                }
                                if (countZoomLevel <= LEVEL08) {
                                    maxZoomOut = true;
                                }
                            }

                            break;
                        case MotionEvent.ACTION_UP:
                            if (!maxZoomOut) {
                                BtnZoomOut.setImageResource(R.drawable.zoom_out_normal);
                            } else {
                                BtnZoomOut.setImageResource(R.drawable.zoom_out_untouchable);
                            }

                            break;
                    }
                }
                interval = ((float) surfaceViewWidth / (float) endPoint);

                return false;
            }
        });
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);

        surfaceViewHeight = findViewById(R.id.surfaceView).getHeight();
        surfaceViewWidth = findViewById(R.id.surfaceView).getWidth();
    }

    static public Set<BluetoothDevice> getPairedDevices() {

        return bluetoothAdapter.getBondedDevices();
    }

    public void DeviceDialog() {
        if (activity.isFinishing()) {

            return;
        }

        FragmentManager fragmentManager = MainActivity.this.getSupportFragmentManager();
        BluetoothList bluetoothList = BluetoothList.newInstance(DEVICES_DIALOG, "");

        bluetoothList.show(fragmentManager, "");

        return;
    }

    public void ErrorDialog(String text) {
        if (activity.isFinishing()) {

            return;
        }

        FragmentManager fragmentManager = MainActivity.this.getSupportFragmentManager();
        BluetoothList bluetoothList = BluetoothList.newInstance(ERROR_DIALOG, text);
        bluetoothList.show(fragmentManager, "");
    }

    public void readData() {
        final Handler handler = new Handler(Looper.getMainLooper());

        stopReading = false;
        readThread = new Thread(new Runnable() {
            public void run() {
                while (!Thread.currentThread().isInterrupted() && !stopReading) {
                    try {
                        int bytesAvailable = inputStream.available();

                        if (bytesAvailable > 0) {
                            byte[] packetBytes = new byte[bytesAvailable];

                            inputStream.read(packetBytes);
                            data = (float) 0.0;
                            for (int i = 0; i < bytesAvailable; i++) {
                                byte    b = packetBytes[i];

                                if (b == '\r') {
                                    handler.post(new Runnable() {
                                        public void run() {
                                            final float value   = ((((float) surfaceViewHeight) * (MAX_VALUE - data)) / MAX_VALUE);

                                            Log.e("Value: " + value, "TAG");
                                            if (!isStreamed) {
                                                //do nothing
                                            } else {
                                                if (currentPoint >= (endPoint - 1)) {
                                                    if (!isScrolled) {
                                                        currentPoint = 0;
                                                        datas[currentPoint] = value;
                                                        currentPoint++;
                                                    } else {
                                                        for (int i = 0; i < currentPoint; i++) {
                                                            datas[i] = datas[i + 1];
                                                        }
                                                        datas[currentPoint] = value;
                                                    }
                                                } else {
                                                    datas[currentPoint] = value;
                                                    currentPoint++;
                                                }
                                                if (!isDrawing) {
                                                    drawThread.setCurrentPoint(currentPoint);
                                                    drawThread.setInterval(interval);
                                                    isDrawing = true;
                                                }
                                            }
                                        }
                                    });

                                    break;
                                } else {
                                    if (data != 0) {
                                        data *= (float) 10.0;
                                    }
                                    data += (float) (b - '0');
                                }
                            }
                        }
                    } catch (IOException e) {
                        stopReading = true;
                    }
                }
            }
        });
        readThread.start();
    }

    public void doConnect(BluetoothDevice device) {
        UUID uuid = UUID.fromString("00001101-0000-1000-8000-00805f9b34fb");

        bluetoothDevice = device;
        try {
            bluetoothSocket = bluetoothDevice.createRfcommSocketToServiceRecord(uuid);
            bluetoothAdapter.cancelDiscovery();
            new ConnectTask().execute();
        } catch (IOException e) {
            ErrorDialog("doConnect" + e.toString());
        }
    }

    public class ConnectTask extends AsyncTask<Void, Void, Object> {

        @Override
        public void onPreExecute() {
            super.onPreExecute();
            progress = new ProgressDialog(MainActivity.this);
            progress.setProgressStyle(ProgressDialog.STYLE_SPINNER);
            progress.setMessage(getString(R.string.wait));
            progress.show();

            return;
        }

        @Override
        public Object doInBackground(Void... params) {
            try {
                bluetoothSocket.connect();
                outputStream = bluetoothSocket.getOutputStream();
                inputStream = bluetoothSocket.getInputStream();
                readData();
                if (drawThread != null) {
                    drawThread = null;
                    drawThread = new DrawThread(surfaceHolder);
                    drawThread.start();
                }
            } catch (Throwable t) {
                doClose();

                return t;
            }

            return null;
        }

        @Override
        public void onPostExecute(Object result) {
            super.onPostExecute(result);
            if (result instanceof Throwable) {
                ErrorDialog("ConnectTask" + result.toString());
            }

            return;
        }

    }

    public void doDisconnect() {
        if (readThread != null) {
            readThread.interrupt();
        }

        new DisconnectTask().execute();
    }

    public void doClose() {
        if (readThread != null) {
            readThread.interrupt();
        }

        new CloseTask().execute();
    }

    public class DisconnectTask extends AsyncTask<Void, Void, Object> {

        @Override
        public void onPreExecute() {
            super.onPreExecute();
            progress = new ProgressDialog(MainActivity.this);
            progress.setProgressStyle(ProgressDialog.STYLE_SPINNER);
            progress.setMessage(getString(R.string.wait));
            progress.show();

            return;
        }

        @Override
        public Object doInBackground(Void... params) {
            try {
                try {
                    outputStream.close();
                } catch (Throwable t) {
                    /*  ignore  */
                }
                try {
                    inputStream.close();
                } catch (Throwable t) {
                    /*  ignore  */
                }
            } catch (Throwable t) {

                return t;
            }

            return null;
        }
    }

    public class CloseTask extends DisconnectTask {

        @Override
        public void onPostExecute(Object result) {
            super.onPostExecute(result);
            if (result instanceof Throwable) {
                ErrorDialog(result.toString());
            }
            progress.dismiss();

            return;
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        unregisterReceiver(checkBluetoothState);
        unregisterReceiver(catchDiscoveredDevices);
        unregisterReceiver(checkACLState);

        return;
    }

    public void showToast(String msg) {
        Toast.makeText(getApplicationContext(), msg, Toast.LENGTH_SHORT).show();

        return;
    }

    public void prepareBluetoothOff() {
        isActivated = false;
        isPaired    = false;
        showToast(getString(R.string.bluetoothNotEnabled));
        btnConnect.setImageResource(R.drawable.connect_untouchable);
        btnStream.setImageResource(R.drawable.start_streaming_untouchable);
        btnScroll.setImageResource(R.drawable.realtime_untouchable);
        BtnZoomIn.setImageResource(R.drawable.zoom_in_untouchable);
        BtnZoomOut.setImageResource(R.drawable.zoom_out_untouchable);
    }

    public void prepareBluetoothOn() {
        isActivated = true;
        isPaired    = false;
        btnConnect.setImageResource(R.drawable.connect_normal);
        btnStream.setImageResource(R.drawable.start_streaming_untouchable);
        btnScroll.setImageResource(R.drawable.realtime_untouchable);
        BtnZoomIn.setImageResource(R.drawable.zoom_in_untouchable);
        BtnZoomOut.setImageResource(R.drawable.zoom_out_untouchable);
    }

    public void prepareConnection() {
        isStreamed  = true;
        isScrolled  = false;
        isActivated = true;
        isPaired    = true;
        maxZoomIn   = false;
        maxZoomOut  = false;
        isWorking   = false;
        isDrawing   = false;
        countZoomLevel  = LEVEL05;
        endPoint        = (BASE * (countZoomLevel + 1));
        interval        = ((float) surfaceViewWidth / (float) endPoint);
        currentPoint    = 0;
        btnConnect.setImageResource(R.drawable.disconnect_normal);
        btnStream.setImageResource(R.drawable.stop_streaming_normal);
        btnScroll.setImageResource(R.drawable.realtime_normal);
        BtnZoomIn.setImageResource(R.drawable.zoom_in_normal);
        BtnZoomOut.setImageResource(R.drawable.zoom_out_normal);
    }

    public void prepareDisconnection() {
        isActivated = true;
        isPaired    = false;
        btnConnect.setImageResource(R.drawable.connect_normal);
        btnStream.setImageResource(R.drawable.start_streaming_untouchable);
        btnScroll.setImageResource(R.drawable.realtime_untouchable);
        BtnZoomIn.setImageResource(R.drawable.zoom_in_untouchable);
        BtnZoomOut.setImageResource(R.drawable.zoom_out_untouchable);
    }

    public SurfaceHolder.Callback surfaceListener = new SurfaceHolder.Callback() {

        @Override
        public void surfaceCreated(SurfaceHolder holder) {
            drawThread = new DrawThread(surfaceHolder);
            drawThread.start();
        }

        @Override
        public void surfaceDestroyed(SurfaceHolder holder) {
            drawThread.isExit = true;
            while (true) {
                try {
                    drawThread.join();
                    break;
                } catch (InterruptedException e) {

                }
            }
        }

        @Override
        public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
            if (drawThread != null) {
                drawThread.changeSize(width, height);
            }
        }
    };

    class DrawThread extends Thread {
        boolean         isExit;
        private int     mWidth, mHeight;
        private int     mCurrentPoint;
        private float   mInterval;
        private float   startX, startY, endX, endY;

        DrawThread(SurfaceHolder holder) {
            surfaceHolder = holder;
            isExit = false;
        }

        public void changeSize(int width, int height) {
            mWidth = width;
            mHeight = height;
        }

        public void setCurrentPoint(int cPoint) {
            mCurrentPoint = cPoint;
        }

        public void setInterval(float interval) {
            mInterval = interval;
        }

        public void run() {
            if (isPaired) {
                Canvas  canvas;
                Paint   paint;

                paint   = new Paint();
                paint.setAntiAlias(true);
                paint.setStrokeWidth(1);
                paint.setColor(Color.GREEN);
                paint.setStyle(Paint.Style.STROKE);
                while (!isExit) {
                    synchronized (surfaceHolder) {
                        //canvas.drawColor(Color.BLACK);
                        if (!isDrawing) {
                            //prepare to drawing
                        } else {
                            canvas = surfaceHolder.lockCanvas();
                            if (canvas == null) {
                                break;
                            }
                            canvas.drawColor(Color.BLACK);
                            for (int i = 0; i < currentPoint; i++) {
                                if (i == 0) {
                                    startX  = endX = 0;
                                    startY  = endY = datas[i];
                                } else {
                                    startX  = endX;
                                    startY  = endY;
                                    endX    += mInterval;
                                    endY    = datas[i];
                                }
                                canvas.drawLine(startX, startY, endX, endY, paint);
                            }
                            surfaceHolder.unlockCanvasAndPost(canvas);
                            isDrawing = false;
                        }
                    }

                    try {
                        Thread.sleep(MainActivity.DELAY);
                    } catch (Exception e) {

                    }
                }
            }
        }
    }
}



    /*
    void sendData() throws IOException
    {
        String msg = myTextbox.getText().toString();
        if ( msg.length() == 0 ) return;

        msg += "\n";
        mmOutputStream.write(msg.getBytes());
        myLabel.setText("Data Sent");
        myTextbox.setText(" ");
    }


    public void drawGraticule() {
        int     widthMaximum, heightMaximum;
        Paint   paint;

        paint = new Paint();
        paint.setAntiAlias(true);
        paint.setStrokeWidth(1);
        paint.setStyle(Paint.Style.STROKE);
        widthMaximum = surfaceView.getRight();
        for (int i = surfaceView.getLeft(); i < widthMaximum; i++) {

        }
        heightMaximum = surfaceView.getTop();
        for (int i = surfaceView.getBottom(); i < heightMaximum; i++) {

        }
    }
     */


/*
public void beginListenForData() {
        final Handler handler = new Handler(Looper.getMainLooper());

        stopReading = false;
        readBufferPosition = 0;
        readBuffer = new byte[1024];
        readThread = new Thread(new Runnable() {
            public void run() {
                while (!Thread.currentThread().isInterrupted() && !stopReading) {
                    try {
                        int bytesAvailable = inputStream.available();

                        if (bytesAvailable > 0) {
                            byte[] packetBytes = new byte[bytesAvailable];
                            inputStream.read(packetBytes);
                            for (int i = 0; i < bytesAvailable; i++) {
                                byte b = packetBytes[i];
                                if (b == '\r') {
                                    byte[]  encodedBytes = new byte[readBufferPosition];

                                    System.arraycopy(readBuffer, 0, encodedBytes, 0, encodedBytes.length);
                                    final String data = new String(encodedBytes, "US-ASCII");
                                    readBufferPosition = 0;
                                    handler.post(new Runnable() {
                                        public void run() {
                                            float   value;

                                            value   = ((((float) surfaceViewHeight) * (MAX_VALUE - Float.parseFloat(data))) / MAX_VALUE);
                                            Log.d("value: " + String.valueOf(value), "TAG");

                                            if (!isStreamed) {
                                                //do nothing
                                            } else {
                                                if (currentPoint >= (endPoint - 1)) {
                                                    if (!isScrolled) {
                                                        currentPoint = 0;
                                                        datas[currentPoint] = value;
                                                        currentPoint++;
                                                    } else {
                                                        for (int i = 0; i < currentPoint; i++) {
                                                            datas[i] = datas[i + 1];
                                                        }
                                                        datas[currentPoint] = value;
                                                    }
                                                } else {
                                                    datas[currentPoint] = value;
                                                    currentPoint++;
                                                }
                                                if (!isDrawing) {
                                                    drawThread.setCurrentPoint(currentPoint);
                                                    drawThread.setInterval(interval);
                                                    isDrawing = true;
                                                }
                                            }
                                        }
                                    });
                                } else {
                                    readBuffer[readBufferPosition++] = b;
                                }
                            }
                        }
                    } catch (IOException e) {
                        stopReading = true;
                    }
                }
            }
        });

        readThread.start();
    }
 */