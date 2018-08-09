package com.turingapi.turingstory;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.Toast;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.InputStream;

public class MainActivity extends AppCompatActivity {

    private BookRecognizer mRecognizer = new BookRecognizer();
    String [] mTestImages = {
            "a_1.jpg", "a_2.jpg", "a_3.jpg", "a_4.jpg", "a_5.jpg",
            "b_1.jpg", "b_2.jpg", "b_3.jpg", "b_4.jpg", "b_5.jpg",
            "c_1.jpg", "c_2.jpg", "c_3.jpg", "c_4.jpg", "c_5.jpg",
            "d_1.jpg", "d_2.jpg", "d_3.jpg", "d_4.jpg", "d_5.jpg",
            "e_1.jpg", "e_2.jpg", "e_3.jpg", "e_4.jpg", "e_5.jpg"
    };

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
            long t1 = System.currentTimeMillis();
            Bitmap img = BitmapFactory.decodeStream(inputstream);
            int num = mRecognizer.queryBook(img);
            long delta = System.currentTimeMillis() - t1;
            Log.e("TuringStory", "img "+path+", result "+num+", cost "+ delta+" ms.");
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        boolean bSuccessful = mRecognizer.init();
        if(bSuccessful) {
            test();
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mRecognizer.free();
    }

}
