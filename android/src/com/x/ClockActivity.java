package com.x;

import org.qtproject.qt5.android.bindings.QtActivity;

import android.app.AlertDialog;
import android.app.Service;
import android.content.DialogInterface;
import android.graphics.Color;
import android.media.MediaPlayer;
import android.os.Build;
import android.os.Bundle;
import android.app.Activity;
import android.appwidget.AppWidgetManager;
import android.content.Context;
import android.appwidget.AppWidgetProvider;
import android.util.Log;
import android.view.WindowManager;

//import android.support.v7.app.AppCompatActivity;
//import androidx.appcompat.app.AppCompatActivity;

public class ClockActivity extends Activity {

    private MediaPlayer mediaPlayer;

    private void setStatusBarColor(String color) {
        // 需要安卓版本大于5.0以上
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            getWindow().addFlags(WindowManager.LayoutParams.FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS);
            getWindow().setStatusBarColor(Color.parseColor(color));
        }
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        //this.setStatusBarColor("#F3F3F3");  //灰

        //mediaPlayer = mediaPlayer.create(this, R.raw.audio);
        //mediaPlayer.start();

        new AlertDialog.Builder(ClockActivity.this).setTitle("Todo").setMessage("提示内容")
                .setPositiveButton("Close", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        //mediaPlayer.stop();
                        ClockActivity.this.finish();
                    }
                }).show();

        System.out.println("闹钟已开始+++++++++++++++++++++++");
    }

}
