package com;

import org.qtproject.qt5.android.bindings.QtActivity;
import android.os.Bundle;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.IBinder;
import com.MyService;

import android.app.PendingIntent;
import android.util.Log;
import android.os.Build;
import android.graphics.Color;
import android.view.WindowManager;
import android.view.View;

public class MyActivity extends QtActivity {

    private static MyActivity m_instance ;

    public  MyActivity()
    {
        m_instance = this;
    }

   public static  int  mini()
    {
        System.out.println("+++++++++++++++++++++++");

        m_instance.moveTaskToBack(true);

        return 1;
    }

//------------------------------------------------------------------------
    private final static String TAG = "QtFullscreen";
    private static Context context;

    //全局获取Context
    public static Context getContext() {
        return context;
    }

    //全透状态栏
    private void setStatusBarFullTransparent()
    {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            getWindow().addFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS);//透明状态栏
            // 状态栏字体设置为深色，SYSTEM_UI_FLAG_LIGHT_STATUS_BAR 为SDK23增加
            getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN | View.SYSTEM_UI_FLAG_LIGHT_STATUS_BAR);

            // 部分机型的statusbar会有半透明的黑色背景
            getWindow().addFlags(WindowManager.LayoutParams.FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS);
            getWindow().clearFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS);
            getWindow().setStatusBarColor(Color.TRANSPARENT);// SDK21
        }
    }

        // 非全透,带颜色的状态栏,需要指定颜色
        private void setStatusBarColor(String color){
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                        // 需要安卓版本大于5.0以上
                        getWindow().addFlags(WindowManager.LayoutParams.FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS);
                        getWindow().setStatusBarColor(Color.parseColor(color));
        }
     }

//----------------------------------------------------------------------

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        //状态栏
        // 获取程序句柄
        context = getApplicationContext();

        // 设置状态栏颜色,需要安卓版本大于5.0
       // this.setStatusBarColor("#FF4040"); //红
        this.setStatusBarColor("#DDDDDD");  //灰

        // 设置状态栏全透明
        //	this.setStatusBarFullTransparent();

        //状态栏文字自适应
        getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_LIGHT_STATUS_BAR);

        //服务
        Intent bindIntent = new Intent(MyActivity.this, MyService.class);
        bindService(bindIntent, mCon, Context.BIND_AUTO_CREATE);
        startService(new Intent(bindIntent));

    }

    private static ServiceConnection mCon = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName arg0, IBinder arg1) {
            // TODO Auto-generated method stub
        }

        @Override
        public void onServiceDisconnected(ComponentName arg0) {
            // TODO Auto-generated method stub

        }
    };
}
