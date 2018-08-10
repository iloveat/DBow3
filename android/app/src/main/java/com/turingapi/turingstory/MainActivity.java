package com.turingapi.turingstory;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceView;
import android.widget.TextView;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.InputStream;

public class MainActivity extends AppCompatActivity implements CameraUtil.OnPreviewFrameListener {

    private TextView mTextInfo = null;
    private TextView mTextName = null;
    private BookRecognizer mRecognizer = new BookRecognizer();
    private boolean mbSuccessful = false;
    private String [] mTestImages = {
            "a_1.jpg", "a_2.jpg", "a_3.jpg", "a_4.jpg", "a_5.jpg",
            "b_1.jpg", "b_2.jpg", "b_3.jpg", "b_4.jpg", "b_5.jpg",
            "c_1.jpg", "c_2.jpg", "c_3.jpg", "c_4.jpg", "c_5.jpg",
            "d_1.jpg", "d_2.jpg", "d_3.jpg", "d_4.jpg", "d_5.jpg",
            "e_1.jpg", "e_2.jpg", "e_3.jpg", "e_4.jpg", "e_5.jpg"
    };

    private void showResult(Bitmap img) {
        long t1 = System.currentTimeMillis();
        int num = mRecognizer.queryBook(img);
        float[] scores = mRecognizer.getScores();
        long delta = System.currentTimeMillis() - t1;
        String name = (String) ResourcesUtil.getIdx2BookIdMap().get(num);
        mTextInfo.setText(String.format("idx: %d\ncost: %d\nscores: \n%.7f\n%.7f\n%.7f", num, delta, scores[0], scores[1], scores[2]));
        //Log.e("TuringStory", String.format("idx: %d\ncost: %d\nscores: \n%.7f\n%.7f\n%.7f", num, delta, scores[0], scores[1], scores[2]));
        mTextName.setText(name);
    }

    public void test() {
        for(String path: mTestImages) {
            String im_path = Environment.getExternalStorageDirectory().getAbsolutePath() + "/images/"+path;
            Uri imgUri = Uri.fromFile(new File(im_path));
            InputStream inputstream = null;
            try {
                inputstream = getContentResolver().openInputStream(imgUri);
            } catch (FileNotFoundException e) {
                e.printStackTrace();
            }
            Bitmap img = BitmapFactory.decodeStream(inputstream);
            showResult(img);
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mTextInfo = (TextView) findViewById(R.id.tv_info);
        mTextName = (TextView) findViewById(R.id.tv_name);

        SurfaceView surfaceView = (SurfaceView) findViewById(R.id.surfaceView);
        CameraUtil camUtil = new CameraUtil(getApplicationContext());
        CameraUtil.mbBackCamera = true;
        camUtil.openCamera(surfaceView, MainActivity.this);

        mbSuccessful = mRecognizer.init();
    }

    @Override
    public void onPreviewFrame(Bitmap bitmap) {
        if(bitmap == null) {
            return;
        }
        if(mbSuccessful) {
            showResult(bitmap);
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mRecognizer.free();
    }

}
