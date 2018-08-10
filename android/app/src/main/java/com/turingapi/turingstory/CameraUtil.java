package com.turingapi.turingstory;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.ImageFormat;
import android.hardware.Camera;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

/**
 * Created by brycezou on 8/10/18.
 */

public class CameraUtil implements SurfaceHolder.Callback, Camera.PreviewCallback {

    public static boolean mRotate90 = true;
    public static boolean mbBackCamera = true;

    private static final String TAG = "TuringOS_CameraUtil";
    private Camera mCamera = null;
    private Context mContext = null;
    private OnPreviewFrameListener mPreviewListener = null;

    public CameraUtil(Context context) {
        this.mContext = context;
        this.mRotate90 = true;
    }

    public void openCamera(SurfaceView surfaceView, OnPreviewFrameListener onPreviewListener) {
        Log.d(TAG, "camera opened.");
        this.mPreviewListener = onPreviewListener;
        if(surfaceView != null) {
            SurfaceHolder surfaceHolder = surfaceView.getHolder();
            surfaceHolder.addCallback(this);
            surfaceHolder.setType(SurfaceHolder.SURFACE_TYPE_NORMAL);
        }
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        Log.d(TAG, "surface created.");
        releaseCamera();
        try {
            if(mbBackCamera) {
                mCamera = Camera.open(Camera.CameraInfo.CAMERA_FACING_BACK);
            } else {
                mCamera = Camera.open(Camera.CameraInfo.CAMERA_FACING_FRONT);
            }

            Camera.Parameters parameters = mCamera.getParameters();
            //设置camera预览的角度，因为默认图片是倾斜90度的
            if(mRotate90) {
                mCamera.setDisplayOrientation(90);  //90
                parameters.setRotation(90);
            } else {
                mCamera.setDisplayOrientation(0);   //0
                parameters.setRotation(0);
            }

            String brand = android.os.Build.BRAND;
            if("google".equals(brand) && mbBackCamera) {
                mCamera.setDisplayOrientation(270);
                parameters.setRotation(270);
            }
            if("motorola".equals(brand) && !mbBackCamera) {
                mCamera.setDisplayOrientation(270);
                parameters.setRotation(270);
            }

            final int width = 640;
            final int height = 480;
            parameters.setPictureSize(width, height);
            parameters.setPreviewSize(width, height);
            parameters.setPreviewFormat(ImageFormat.NV21);
            if(mbBackCamera) {
                parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE);
                mCamera.setParameters(parameters);
            }
            BitmapUtil.initYuvParams(mContext, width, height);

            mCamera.setPreviewDisplay(holder);
            mCamera.setPreviewCallback(this);
            mCamera.startPreview();
        } catch(Exception e) {
            e.printStackTrace();
        }
    }

    @Override
    public void onPreviewFrame(byte[] data, Camera camera) {
        //Log.e(TAG, "onPreviewFrame");
        Bitmap bitmap = BitmapUtil.yuvToRGB(data);
        if(mRotate90 && mbBackCamera) {
            bitmap = BitmapUtil.rotaingImageView(90, bitmap, 0, 0);  //90
        } else if(mRotate90 && !mbBackCamera) {
            bitmap = BitmapUtil.rotaingImageView(270, bitmap, 1, -1);  //270
        }
        mPreviewListener.onPreviewFrame(bitmap);
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        Log.d(TAG, "surface changed.");
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.d(TAG, "surface destroyed.");
        releaseCamera();
    }

    public void releaseCamera() {
        if(mCamera != null) {
            mCamera.stopPreview();
            mCamera.setPreviewCallback(null);
            mCamera.release();
            mCamera = null;
        }
    }

    public interface OnPreviewFrameListener {
        void onPreviewFrame(Bitmap bitmap);
    }
}