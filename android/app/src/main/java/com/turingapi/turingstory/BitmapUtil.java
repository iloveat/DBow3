package com.turingapi.turingstory;


import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Matrix;
import android.graphics.Rect;
import android.renderscript.Allocation;
import android.renderscript.Element;
import android.renderscript.RenderScript;
import android.renderscript.ScriptIntrinsicYuvToRGB;
import android.renderscript.Type;
import android.util.Log;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;

/**
 * Created by brycezou on 6/13/17.
 */

public class BitmapUtil {

    private static final String TAG = "BitmapUtil";

    private static Bitmap mBitmap = null;
    private static Allocation mIn = null;
    private static Allocation mOut = null;
    private static ScriptIntrinsicYuvToRGB mYuvToRgbIntrinsic = null;

    public static void initYuvParams(Context context, int cameraWidth, int cameraHeight) {
        RenderScript rs = RenderScript.create(context);
        mYuvToRgbIntrinsic = ScriptIntrinsicYuvToRGB.create(rs, Element.RGBA_8888(rs));
        Type.Builder yuvType = new Type.Builder(rs, Element.U8(rs)).setX(cameraWidth).setY(cameraHeight).setYuvFormat(android.graphics.ImageFormat.NV21);
        mIn = Allocation.createTyped(rs, yuvType.create(), Allocation.USAGE_SCRIPT);
        Type.Builder rgbaType = new Type.Builder(rs, Element.RGBA_8888(rs)).setX(cameraWidth).setY(cameraHeight);
        mOut = Allocation.createTyped(rs, rgbaType.create(), Allocation.USAGE_SCRIPT);
        mBitmap = Bitmap.createBitmap(cameraWidth, cameraHeight, Bitmap.Config.ARGB_8888);
    }

    /**将yuv格式的byte数组转化成RGB的bitmap*/
    public static Bitmap yuvToRGB(byte[] data) {
        mIn.copyFrom(data);
        mYuvToRgbIntrinsic.setInput(mIn);
        mYuvToRgbIntrinsic.forEach(mOut);
        mOut.copyTo(mBitmap);
        return mBitmap;
    }

    /**将argb_8888的图像转化为w*h*4的字节数组，算法使用*/
    public static byte[] bitmap2byte(final Bitmap bitmap) {
        ByteBuffer byteBuffer = ByteBuffer.allocate(bitmap.getByteCount());
        bitmap.copyPixelsToBuffer(byteBuffer);
        byte[] bytes = byteBuffer.array();
        return bytes;
    }

    /**将图片按照固定比例进行压缩*/
    public static Bitmap resizeBitmapWithConstantWHRatio(Bitmap bmp, int mWidth, int mHeight) {
        if(bmp != null) {
            Bitmap bitmap = bmp;
            float width = bitmap.getWidth(); //728
            float height = bitmap.getHeight(); //480
            Log.d(TAG, "----原图片的宽度:" +bmp.getWidth()+", 高度:"+bmp.getHeight()); //720/480 = 1.5

            float scale = 1.0f;
            float scaleX = (float)mWidth/width;
            float scaleY = (float)mHeight/height;
            if(scaleX < scaleY && (scaleX > 0 || scaleY > 0)) {
                scale = scaleX;
            }
            if(scaleY <= scaleX &&(scaleX > 0 || scaleY > 0)) {
                scale = scaleY;
            }

            Log.d(TAG, "-----scaleX:" +scale+" , scaleY:"+scale);
            return resizeBitmapByScale(bmp, scale);
        }
        return null;
    }

    public static Bitmap resizeBitmapByScale(Bitmap bitmap, float scale) {
        Matrix matrix = new Matrix();
        matrix.postScale(scale, scale);
        float width = bitmap.getWidth(); //728
        float height = bitmap.getHeight(); //480
        Bitmap bmpOut= Bitmap.createBitmap(bitmap, 0, 0, (int)width, (int)height, matrix, true);
        return bmpOut;
    }

    /***将bitmap写进指定路径*/
    public static String saveToInternalStorage(File imageFile, Bitmap bitmapImage) {
        String path = "";
        if(!imageFile.exists()) {
            try {
                imageFile.createNewFile();
            } catch(IOException e) {
                e.printStackTrace();
            }
        }
        FileOutputStream fos = null;
        try {
            fos = new FileOutputStream(imageFile);
            bitmapImage.compress(Bitmap.CompressFormat.JPEG, 100, fos);
            fos.flush();
            path = imageFile.getAbsolutePath();
        } catch(Exception e) {
            e.printStackTrace();
        } finally {
            try {
                bitmapImage.recycle();
                if(fos != null) {
                    fos.close();
                }
            } catch(IOException e) {
                e.printStackTrace();
            }
        }
        return path; //得到.jpg的全路径名
    }

    /**bitmap转化为byte[]数组，网络传输使用*/
    public static byte[] bitmap2Bytes(Bitmap bitmap) {
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        bitmap.compress(Bitmap.CompressFormat.JPEG, 30, baos);
        return baos.toByteArray();
    }

    /**sx, sy -1, 1   左右翻转   1, -1  上下翻转*/
    public static Bitmap rotaingImageView(int angle, Bitmap srcBitmap, float sx, float sy) {
        Matrix matrix = new Matrix();  //使用矩阵 完成图像变换
        if(sx != 0 || sy != 0) {
            matrix.postScale(sx, sy);  //重点代码，记住就ok
        }

        int w = srcBitmap.getWidth();
        int h = srcBitmap.getHeight();
        Bitmap cacheBitmap = Bitmap.createBitmap(w, h, Bitmap.Config.ARGB_8888);
        Canvas canvas = new Canvas(cacheBitmap);  //使用canvas在bitmap上面画像素

        matrix.postRotate(angle);
        Bitmap retBitmap = Bitmap.createBitmap(srcBitmap, 0, 0, w, h, matrix, true);
        canvas.drawBitmap(retBitmap, new Rect(0, 0, w, h), new Rect(0, 0, w, h), null);
        return retBitmap;
    }

}
