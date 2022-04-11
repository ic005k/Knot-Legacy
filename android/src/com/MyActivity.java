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

import android.os.PowerManager;
import android.os.PowerManager.WakeLock;

import android.content.IntentFilter;
import android.content.BroadcastReceiver;
import android.hardware.SensorManager;
import android.app.Service;
import android.hardware.Sensor;
import android.hardware.SensorEventListener;
import android.hardware.SensorEvent;
import android.os.PersistableBundle;
import android.widget.TextView;

public class MyActivity extends QtActivity {

    private static MyActivity m_instance ;


    private SensorManager mSensorManager;

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
    WakeLock wakeLock=null;
         private void acquireWakeLock_1() {
          if (null == wakeLock) {

              mSensorManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
              mySerivece = new PersistService();

              PowerManager pm = (PowerManager) getSystemService(Context.POWER_SERVICE);
              wakeLock = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK
                      | PowerManager.ON_AFTER_RELEASE, getClass()
                      .getCanonicalName());
              IntentFilter filter = new IntentFilter(Intent.ACTION_SCREEN_ON);// 屏幕熄掉后依然运行
              filter.addAction(Intent.ACTION_SCREEN_OFF);
              registerReceiver(mySerivece.mReceiver, filter);

              if (null != wakeLock) {
                  Log.i(TAG, "call acquireWakeLock");
                  wakeLock.acquire();
              }
          }
      }
      // 释放设备电源锁
      private void releaseWakeLock_1() {
          if (null != wakeLock && wakeLock.isHeld()) {
              //Log.i(TAG, "call releaseWakeLock");
              wakeLock.release();
              wakeLock = null;
          }
      }

//-----------------------------------------------------------------------
    //private static final String TAG = "AccleratePersist";
    private WakeLock mWakeLock;
    private PersistService mySerivece;
    private static final int DELAY = SensorManager.SENSOR_DELAY_NORMAL;
    public void acquireWakeLock() {
    //final PowerManager powerManager = (PowerManager) getSystemService(Context.POWER_SERVICE);
    releaseWakeLock();

   mSensorManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
   mySerivece = new PersistService();
   PowerManager manager = (PowerManager) getSystemService(Context.POWER_SERVICE);
   mWakeLock = manager.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, TAG);// CPU保存运行
   IntentFilter filter = new IntentFilter(Intent.ACTION_SCREEN_ON);// 屏幕熄掉后依然运行
   filter.addAction(Intent.ACTION_SCREEN_OFF);
   registerReceiver(mySerivece.mReceiver, filter);

   mWakeLock.acquire();// 屏幕熄后，CPU继续运行
   mSensorManager.registerListener(
                  mySerivece,
                  mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER),
                  DELAY);
}

public void releaseWakeLock() {

    if (mWakeLock != null && mWakeLock.isHeld()) {
        mSensorManager.unregisterListener(mySerivece);
        mWakeLock.release();
        mWakeLock = null;
        Log.i(TAG, "call releaseWakeLock");
    }
}

//-----------------------------------------------------------------------


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        //电源锁
        acquireWakeLock();

        //状态栏
        // 获取程序句柄
        context = getApplicationContext();

       // 设置状态栏颜色,需要安卓版本大于5.0
       // this.setStatusBarColor("#FF4040"); //红
        this.setStatusBarColor("#DDDDDD");  //灰

        // 设置状态栏全透明
        // this.setStatusBarFullTransparent();

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

    @Override
    public void onPause() {
        System.out.println("Pause...");
        super.onPause();

        }

    @Override
    public void onStop() {
        System.out.println("Stop...");
        super.onStop();

        }

    @Override
    protected void onDestroy() {
    Log.i(TAG, "onDestroy...");
    releaseWakeLock();
    super.onDestroy();


    }
//---------------------------------------------------------------------------
    private static boolean STOP = false;// 开始暂停按钮
    private StringBuilder builder = new StringBuilder();
    private TextView accView;// 显示加速度值
    private long lastTimestamp = 0;
    class PersistService extends Service implements SensorEventListener {

        private float[] accValue = new float[3];

        public BroadcastReceiver mReceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {

                if (!STOP
                        && !intent.getAction().equals(Intent.ACTION_SCREEN_OFF)) {
                    return;
                }
                if (mSensorManager != null) {//取消监听后重写监听，以保持后台运行
                    mSensorManager.unregisterListener(PersistService.this);
                    mSensorManager
                            .registerListener(
                                    PersistService.this,
                                    mSensorManager
                                            .getDefaultSensor(Sensor.TYPE_ACCELEROMETER),
                                    SensorManager.SENSOR_DELAY_NORMAL);
                }

            }

        };

        public void onAccuracyChanged(Sensor sensor, int accuracy) {
            Log.i(TAG, "PersistService.onAccuracyChanged().");
        }

        public void onSensorChanged(SensorEvent sensorEvent) {
            if (sensorEvent.sensor.getType() == Sensor.TYPE_ACCELEROMETER) {
                Log.i(TAG, "PersistService.TYPE_ACCELEROMETER.");
                accValue = sensorEvent.values;
                for (int i = 0; i < 3; i++) {
                    builder.append((int) accValue[i]);
                    builder.append(",");
                }
                builder.append((sensorEvent.timestamp - lastTimestamp) / 1000000);// 采样时间差
                builder.append("\n");
                //accView.setText(builder.toString());
                lastTimestamp = sensorEvent.timestamp;
            }

        }
        @Override
        public IBinder onBind(Intent intent) {
            // TODO Auto-generated method stub
            return null;
        }

    }
}


