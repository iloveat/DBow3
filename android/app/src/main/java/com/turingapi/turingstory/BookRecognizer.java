package com.turingapi.turingstory;

import android.graphics.Bitmap;
import android.os.Environment;
import android.util.Log;

import java.nio.ByteBuffer;

/**
 * Created by brycezou on 8/9/18.
 */

public class BookRecognizer {

    private static final String TAG = "TuringStory";
    private static String mDbPath = null;

    public BookRecognizer() {
        mDbPath = Environment.getExternalStorageDirectory().getAbsolutePath()+"/small_db.yml.gz";
    }

    public boolean init() {
        return jniInit(mDbPath);
    }

    public boolean free() {
        return jniFree();
    }

    public int setNumFeatures(int num) {
        int ret = jniSetNumFeatures(num);
        if(ret < 0) {
            Log.e(TAG, "Please call init() first.");
        }
        return ret;
    }

    public int setThreshold(float thresh) {
        int ret = jniSetThreshold(new float[]{ thresh });
        if(ret < 0) {
            Log.e(TAG, "Please call init() first.");
        }
        return ret;
    }

    public int queryBook(Bitmap bitmap) {
        setThreshold(0.12f);
        byte[] bytes = bitmap2byte(bitmap);
        int width = bitmap.getWidth();
        int height = bitmap.getHeight();
        return jniQueryBook(bytes, width, height);
    }

    public float[] getScores() {
        return jniGetScores();
    }

    private byte[] bitmap2byte(final Bitmap bitmap) {
        ByteBuffer byteBuffer = ByteBuffer.allocate(bitmap.getByteCount());
        bitmap.copyPixelsToBuffer(byteBuffer);
        return byteBuffer.array();
    }

    static {
        try {
            System.loadLibrary("opencv_java3");
            System.loadLibrary("turing_story");
        } catch (UnsatisfiedLinkError e) {
            Log.d(TAG, "turing story library not found");
        }
    }
    private native static boolean jniInit(String str_db_path);
    private native static boolean jniFree();
    private native static int jniSetNumFeatures(int num);
    private native static int jniSetThreshold(float []thresh);
    private native static int jniQueryBook(byte []image, int width, int height);
    private native static float[] jniGetScores();
}
