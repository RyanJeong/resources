package com.example.user.doorlockclient;

import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import android.view.WindowManager;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.MatOfRect;
import org.opencv.core.Point;
import org.opencv.core.Rect;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.imgproc.Imgproc;
import org.opencv.objdetect.CascadeClassifier;
import org.opencv.objdetect.Objdetect;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

/**
 * Created by user on 2016-10-15.
 */

public class InsertPattern extends PrepareView implements CameraBridgeViewBase.CvCameraViewListener2 {
    private static final Scalar FACE_RECT_COLOR = new Scalar(0, 255, 0, 255);
    private int learn_frames = 0;
    private int inputCount;
    private String[] mDetectorName;
    public static final int JAVA_DETECTOR = 0;
    private Mat templateR;
    private Mat templateL;
    private File mCascadeFile;
    private File mCascadeFileEye;
    private CascadeClassifier mJavaDetector;
    private CascadeClassifier mJavaDetectorEye;
    private float mRelativeFaceSize = 0.2f;
    private int mAbsoluteFaceSize = 0;
    private double xCenter = (-1);
    private double yCenter = (-1);
    private Mat mRgba;
    private Mat mGray;
    private CameraBridgeViewBase mOpenCvCameraView;
    private BaseLoaderCallback mLoaderCallback = new BaseLoaderCallback(this) {

        @Override
        public void onManagerConnected(int status) {
            switch (status) {
                case LoaderCallbackInterface.SUCCESS:
                    Log.i(tag, "[InsertPattern] OpenCV loaded successfully");
                    try {
                        // load cascade file from application resources
                        InputStream is = getResources().openRawResource(R.raw.lbpcascade_frontalface);
                        File cascadeDir = getDir("cascade", Context.MODE_PRIVATE);
                        mCascadeFile = new File(cascadeDir, "lbpcascade_frontalface.xml");
                        FileOutputStream os = new FileOutputStream(mCascadeFile);

                        byte[] buffer = new byte[4096];
                        int bytesRead;
                        while ((bytesRead = is.read(buffer)) != -1) {
                            os.write(buffer, 0, bytesRead);
                        }
                        is.close();
                        os.close();
                        // load cascade file from application resources
                        InputStream ise = getResources().openRawResource(R.raw.haarcascade_lefteye_2splits);
                        File cascadeDirEye = getDir("cascade", Context.MODE_PRIVATE);
                        mCascadeFileEye = new File(cascadeDirEye, "haarcascade_lefteye_2splits.xml");
                        FileOutputStream ose = new FileOutputStream(mCascadeFileEye);

                        while ((bytesRead = ise.read(buffer)) != -1) {
                            ose.write(buffer, 0, bytesRead);
                        }
                        ise.close();
                        ose.close();

                        mJavaDetector = new CascadeClassifier(mCascadeFile.getAbsolutePath());
                        if (mJavaDetector.empty()) {
                            Log.i(tag, "[InsertPattern] Failed to load cascade classifier");
                            mJavaDetector = null;
                        } else {
                            showLog(tag, "[InsertPattern] Loaded cascade classifier from " + mCascadeFile.getAbsolutePath());
                        }
                        mJavaDetectorEye = new CascadeClassifier(mCascadeFileEye.getAbsolutePath());
                        if (mJavaDetectorEye.empty()) {
                            showLog(tag, "[InsertPattern] Failed to load cascade classifier for eye");
                            mJavaDetectorEye = null;
                        } else {
                            showLog(tag, "[InsertPattern] Loaded cascade classifier from " + mCascadeFileEye.getAbsolutePath());
                        }
                        cascadeDir.delete();
                        cascadeDirEye.delete();
                    } catch (IOException e) {
                        e.printStackTrace();
                        showLog(tag, "[InsertPattern] Failed to load cascade. Exception thrown: " + e);
                    }
                    mOpenCvCameraView.enableFpsMeter();
                    mOpenCvCameraView.setCameraIndex(CameraBridgeViewBase.CAMERA_ID_FRONT);
                    mOpenCvCameraView.enableView();
                    break;
                default:
                    super.onManagerConnected(status);
                    break;
            }

            return;
        }
    };

    public InsertPattern() {
        mDetectorName = new String[2];
        mDetectorName[JAVA_DETECTOR] = "Java";

        showLog(tag, "[InsertPattern] Instantiated new " + this.getClass());
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.insert_pattern);

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        mOpenCvCameraView = (CameraBridgeViewBase) findViewById(R.id.activity_surface_view);
        mOpenCvCameraView.setCvCameraViewListener(this);

        return;
    }

    @Override
    public void onPause() {
        super.onPause();
        if (mOpenCvCameraView != null) {
            mOpenCvCameraView.disableView();
        }

        return;
    }

    @Override
    public void onResume() {
        super.onResume();
        if (!OpenCVLoader.initDebug()) {
            showLog(tag, "[InsertPattern] Internal OpenCV library not found.");
            OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_3_0_0, this, mLoaderCallback);
        } else {
            showLog(tag, "[InsertPattern] OpenCV library found inside package. Using it!");
            mLoaderCallback.onManagerConnected(LoaderCallbackInterface.SUCCESS);
        }

        return;
    }

    public void onDestroy() {
        super.onDestroy();
        if (mOpenCvCameraView != null) {
            mOpenCvCameraView.disableView();
        }

        return;
    }

    @Override
    public void onCameraViewStarted(int width, int height) {
        mGray = new Mat();
        mRgba = new Mat();
        try {
            showLog(tag, "[InsertPattern] Set READY");
            SocketManager.writeMsg(msgReady);
            inputCount = 0;
        } catch (IOException e) {
            showLog(tag, "[InsertPattern] set Ready failed.");
            e.printStackTrace();
        }

        return;
    }

    @Override
    public void onCameraViewStopped() {
        mGray.release();
        mRgba.release();

        return;
    }

    @Override
    public Mat onCameraFrame(CameraBridgeViewBase.CvCameraViewFrame inputFrame) {
        mRgba = inputFrame.rgba();
        mGray = inputFrame.gray();
        if (mAbsoluteFaceSize == 0) {
            int height = mGray.rows();

            if (Math.round(height * mRelativeFaceSize) > 0) {
                mAbsoluteFaceSize = Math.round(height * mRelativeFaceSize);
            }
        }
        MatOfRect faces = new MatOfRect();
        if ((mJavaDetector != null)) {
            mJavaDetector.detectMultiScale(mGray, faces, 1.1, 2, 2, new Size(mAbsoluteFaceSize, mAbsoluteFaceSize), new Size());
        }
        Rect[] facesArray = faces.toArray();
        for (int i = 0; i < facesArray.length; i++) {
            Imgproc.rectangle(mRgba, facesArray[i].tl(), facesArray[i].br(), FACE_RECT_COLOR, 3);
            xCenter = ((facesArray[i].x + facesArray[i].width + facesArray[i].x) / 2);
            yCenter = ((facesArray[i].y + facesArray[i].y + facesArray[i].height) / 2);
            Point center = new Point(xCenter, yCenter);
            Imgproc.circle(mRgba, center, 10, new Scalar(255, 0, 0, 255), 3);
            Imgproc.putText(mRgba, "[" + center.x + "," + center.y + "]", new Point((center.x + 20), (center.y + 20)),
                    Core.FONT_HERSHEY_SIMPLEX, 0.7, new Scalar(255, 255, 255, 255));
            Rect r = facesArray[i];
            showLog(tag, "[InsertPattern] Compute the eye area");
            Rect eyearea = new Rect((r.x + (r.width / 8)),
                    (int) (r.y + (r.height / 4.5)), (r.width - 2 * (r.width / 8)),
                    (int) (r.height / 3.0));
            // split it
            showLog(tag, "[InsertPattern] eyearea_right");
            Rect eyearea_right = new Rect((r.x + (r.width / 16)),
                    (int) (r.y + (r.height / 4.5)),
                    (r.width - 2 * (r.width) / 16) / 2, (int) (r.height / 3.0));
            showLog(tag, "[InsertPattern] eyearea_left");
            Rect eyearea_left = new Rect(((r.x + (r.width / 16))
                    + (r.width - 2 * (r.width) / 16) / 2),
                    (int) (r.y + (r.height / 4.5)),
                    ((r.width - 2 * (r.width) / 16) / 2), (int) (r.height / 3.0));
            // draw the area - mGray is working grayscale mat, if you want to
            // see area in rgb preview, change mGray to mRgba
            Imgproc.rectangle(mRgba, eyearea_left.tl(), eyearea_left.br(), new Scalar(255, 0, 0, 255), 2);
            Imgproc.rectangle(mRgba, eyearea_right.tl(), eyearea_right.br(), new Scalar(255, 0, 0, 255), 2);
            if (learn_frames < 5) {
                templateR = get_template(mJavaDetectorEye, eyearea_right, 24);
                templateL = get_template(mJavaDetectorEye, eyearea_left, 24);
                showLog(tag, "[InsertPattern] LEARN_FRAMES < 5.");
                learn_frames++;
            } else {
                showLog(tag, "[InsertPattern] LEARNING Finished");
                match_eye(eyearea_right, templateR);
                match_eye(eyearea_left, templateL);
            }
        }

        return mRgba;
    }

    private void match_eye(Rect area, Mat mTemplate) {
        showLog(tag, "[InsertPattern] MATCH_EYE");
        Point matchLoc;
        Mat mROI = mGray.submat(area);
        int result_cols = (mROI.cols() - mTemplate.cols() + 1);
        int result_rows = (mROI.rows() - mTemplate.rows() + 1);
        // Check for bad template size
        if (mTemplate.cols() == 0 || mTemplate.rows() == 0) {

            return ;
        }
        Mat mResult = new Mat(result_cols, result_rows, CvType.CV_8U);
        /*
                switch (type) {
                    case TM_SQDIFF:
                        Imgproc.matchTemplate(mROI, mTemplate, mResult, Imgproc.TM_SQDIFF);
                        break;
                    case TM_SQDIFF_NORMED:
                        Imgproc.matchTemplate(mROI, mTemplate, mResult,
                                Imgproc.TM_SQDIFF_NORMED);
                        break;
                    case TM_CCOEFF:
                        Imgproc.matchTemplate(mROI, mTemplate, mResult, Imgproc.TM_CCOEFF);
                        break;
                    case TM_CCOEFF_NORMED:
                        Imgproc.matchTemplate(mROI, mTemplate, mResult,
                                Imgproc.TM_CCOEFF_NORMED);
                        break;
                    case TM_CCORR:
                        Imgproc.matchTemplate(mROI, mTemplate, mResult, Imgproc.TM_CCORR);
                        break;
                    case TM_CCORR_NORMED:
                        Imgproc.matchTemplate(mROI, mTemplate, mResult,
                                Imgproc.TM_CCORR_NORMED);
                        break;
                }
         */
        Imgproc.matchTemplate(mROI, mTemplate, mResult, Imgproc.TM_CCOEFF_NORMED);
        Core.MinMaxLocResult mmres = Core.minMaxLoc(mResult);
        // there is difference in matching methods - best match is max/min value
        /*
        if (type == TM_SQDIFF || type == TM_SQDIFF_NORMED) {
            matchLoc = mmres.minLoc;
        } else {
            matchLoc = mmres.maxLoc;
        }
         */
        matchLoc = mmres.maxLoc;
        Point matchLoc_tx = new Point(matchLoc.x + area.x, matchLoc.y + area.y);
        Point matchLoc_ty = new Point(matchLoc.x + mTemplate.cols() + area.x, matchLoc.y + mTemplate.rows() + area.y);
        Imgproc.rectangle(mRgba, matchLoc_tx, matchLoc_ty, new Scalar(255, 255, 0, 255));
        Rect rec = new Rect(matchLoc_tx,matchLoc_ty);
    }

    private Mat get_template(CascadeClassifier clasificator, Rect area, int size) {
        showLog(tag, "[InsertPattern] GET_TEMPLATE");
        Mat template = new Mat();
        Mat mROI = mGray.submat(area);
        MatOfRect eyes = new MatOfRect();
        Point iris = new Point();
        Rect eye_template = new Rect();
        clasificator.detectMultiScale(mROI, eyes, 1.15, 2,
                Objdetect.CASCADE_FIND_BIGGEST_OBJECT | Objdetect.CASCADE_SCALE_IMAGE, new Size(30, 30), new Size());
        Rect[] eyesArray = eyes.toArray();
        for (int i = 0; i < eyesArray.length; i++) {
            showLog(tag, "[InsertPattern] GET_TEMPLATE -> For loop");
            Rect e = eyesArray[i];
            e.x = (area.x + e.x);
            e.y = (area.y + e.y);
            Rect eye_only_rectangle = new Rect((int) e.tl().x,
                    (int) (e.tl().y + e.height * 0.3), (int) e.width,
                    (int) (e.height * 0.5));
            mROI = mGray.submat(eye_only_rectangle);
            Mat vyrez = mRgba.submat(eye_only_rectangle);
            Core.MinMaxLocResult mmG = Core.minMaxLoc(mROI);
            Imgproc.circle(vyrez, mmG.minLoc, 2, new Scalar(255, 255, 255, 255), 2);
            iris.x = mmG.minLoc.x + eye_only_rectangle.x;
            iris.y = mmG.minLoc.y + eye_only_rectangle.y;
            eye_template = new Rect((int) (iris.x - (size / 2)), (int) (iris.y - (size / 2)), size, size);
            Imgproc.rectangle(mRgba, eye_template.tl(), eye_template.br(), new Scalar(255, 0, 0, 255), 2);
            template = (mGray.submat(eye_template)).clone();

            return template;
        }

        return template;
    }
}
