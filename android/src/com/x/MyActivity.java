package com.x;

import org.qtproject.qt5.android.bindings.QtActivity;

import android.os.Bundle;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.IBinder;

import com.x.MyService;

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

import java.util.Timer;
import java.util.TimerTask;

public class MyActivity extends QtActivity {

    private static MyActivity m_instance;
    private static SensorManager mSensorManager;
    public static int isStepCounter = -1;
    public static float stepCounts;
    private static WakeLock mWakeLock = null;
    private static PersistService mySerivece;
    private static final int DELAY = SensorManager.SENSOR_DELAY_NORMAL;

    public native void CallJavaNotify_1();

    public native void CallJavaNotify_2();

    public MyActivity() {
        m_instance = this;
    }

    public static int mini() {
        System.out.println("Mini+++++++++++++++++++++++");
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
    private void setStatusBarFullTransparent() {
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
    private void setStatusBarColor(String color) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            // 需要安卓版本大于5.0以上
            getWindow().addFlags(WindowManager.LayoutParams.FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS);
            getWindow().setStatusBarColor(Color.parseColor(color));
        }
    }

    //----------------------------------------------------------------------
    public void initSensor() {
        mSensorManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
        mySerivece = new PersistService();
        //PowerManager manager = (PowerManager) getSystemService(Context.POWER_SERVICE);
        //mWakeLock = manager.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, TAG);// CPU保存运行
        //IntentFilter filter = new IntentFilter(Intent.ACTION_SCREEN_ON);// 屏幕熄掉后依然运行
        //filter.addAction(Intent.ACTION_SCREEN_OFF);
        //registerReceiver(mySerivece.mReceiver, filter);

        //mWakeLock.acquire();// 屏幕熄后，CPU继续运行
        /*mSensorManager.registerListener(
                mySerivece,
                mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER),
                DELAY);*/
        Sensor countSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_STEP_COUNTER);
        if (countSensor != null) {
            mSensorManager.registerListener(
                    mySerivece,
                    countSensor,
                    DELAY);
            isStepCounter = 1;
        } else
            isStepCounter = 0;
    }

    private SensorManager mySensorManager;
    private Sensor countSensor;

    public void initStepSensor() {

        if (countSensor != null) {
            /*mySensorManager.registerListener(new SensorEventListener() {
                @Override
                public void onSensorChanged(SensorEvent event) {
                    if (event.sensor.getType() == Sensor.TYPE_STEP_COUNTER) {
                        //float[] values = event.values;

                        stepCounts =(long)event.values[0];
                    }

                }

                @Override
                public void onAccuracyChanged(Sensor sensor, int accuracy) {

                }
            }, countSensor, SensorManager.SENSOR_DELAY_FASTEST);*/

            if (mySerivece != null) {
                mySensorManager.unregisterListener(mySerivece);
            }

            mySensorManager.registerListener(mySerivece,
                    mySensorManager.getDefaultSensor(Sensor.TYPE_STEP_COUNTER),
                    SensorManager.SENSOR_DELAY_NORMAL);
            isStepCounter = 1;
        } else
            isStepCounter = 0;

    }

    public static int getHardStepCounter() {
        return isStepCounter;
    }

    public static float getSteps() {
        return stepCounts;
    }

    // 屏幕唤醒相关
    private ScreenStatusReceiver mScreenStatusReceiver;

    private void registSreenStatusReceiver() {
        mScreenStatusReceiver = new ScreenStatusReceiver();
        IntentFilter screenStatusIF = new IntentFilter();
        screenStatusIF.addAction(Intent.ACTION_SCREEN_ON);
        screenStatusIF.addAction(Intent.ACTION_SCREEN_OFF);
        registerReceiver(mScreenStatusReceiver, screenStatusIF);
    }

    class ScreenStatusReceiver extends BroadcastReceiver {
        String SCREEN_ON = "android.intent.action.SCREEN_ON";
        String SCREEN_OFF = "android.intent.action.SCREEN_OFF";

        @Override
        public void onReceive(Context context, Intent intent) {
            if (SCREEN_ON.equals(intent.getAction())) {
                if (isStepCounter == 1) {

                    CallJavaNotify_2();
                }
                Log.w("Knot", "屏幕亮了");

            } else if (SCREEN_OFF.equals(intent.getAction())) {
                if (isStepCounter == 1) {
                    if (mySerivece != null) {
                        mySensorManager.unregisterListener(mySerivece);

                    }
                }
                Log.w("Knot", "屏幕熄了");
            }
        }
    }

    //----------------------------------------------------------------------
    public void acquireWakeLock() {
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

        Log.i(TAG, "call acquireWakeLock");
    }

    public static void releaseWakeLock() {

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

        //唤醒锁
        //acquireWakeLock();
        mySerivece = new PersistService();
        mySensorManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
        countSensor = mySensorManager.getDefaultSensor(Sensor.TYPE_STEP_COUNTER);
        initStepSensor();

        registSreenStatusReceiver();

        //状态栏
        context = getApplicationContext();  // 获取程序句柄
        // 设置状态栏颜色,需要安卓版本大于5.0
        // this.setStatusBarColor("#FF4040"); //红
        this.setStatusBarColor("#F3F3F3");  //灰
        // 设置状态栏全透明
        // this.setStatusBarFullTransparent();
        //状态栏文字自适应
        getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_LIGHT_STATUS_BAR);

        //服务
        Intent bindIntent = new Intent(MyActivity.this, MyService.class);
        if (Build.VERSION.SDK_INT >= 26) {
            startForegroundService(bindIntent);

        } else {
            bindService(bindIntent, mCon, Context.BIND_AUTO_CREATE);
            startService(new Intent(bindIntent));
        }

        //MyService.notify(getApplicationContext(), "Hello!");
    }

    private static ServiceConnection mCon = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName arg0, IBinder arg1) {
            // Auto-generated method stub
        }

        @Override
        public void onServiceDisconnected(ComponentName arg0) {
            // Auto-generated method stub

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
        unregisterReceiver(mScreenStatusReceiver);
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

        int count = 0;
        double ax, ay, az;

        public void onSensorChanged(SensorEvent sensorEvent) {

            //if (sensorEvent.sensor.getType() == Sensor.TYPE_ACCELEROMETER) {
                /*Log.i(TAG, "PersistService.TYPE_ACCELEROMETER.");
                accValue = sensorEvent.values;
                ax=accValue[0];
                ay=accValue[1];
                az=accValue[2];
                count++;
                if(count>=5000)
                {
                    count = 0;
                    if(ax==0&&ay==0&&az==0)
                    {
                        regSensor();
                    }

                }*/

                /*for (int i = 0; i < 3; i++) {
                    builder.append((int) accValue[i]);
                    builder.append(",");

                }
                builder.append((sensorEvent.timestamp - lastTimestamp) / 1000000);// 采样时间差
                builder.append("\n");
                //accView.setText(builder.toString());
                Log.i(TAG, builder.toString());*/

            //lastTimestamp = sensorEvent.timestamp;
            //}
            // 做个判断传感器类型很重要，这可以过滤掉杂音（比如可能来自其它传感器的值）
            if (sensorEvent.sensor.getType() == Sensor.TYPE_STEP_COUNTER) {
                //float[] values = sensorEvent.values;
                stepCounts = (long) sensorEvent.values[0];
            }
        }

        @Override
        public IBinder onBind(Intent intent) {
            // Auto-generated method stub
            return null;
        }

        public void regSensor() {
            if (mSensorManager != null) {//取消监听后重写监听，以保持后台运行
                mSensorManager.unregisterListener(PersistService.this);
                mSensorManager
                        .registerListener(
                                PersistService.this,
                                mSensorManager
                                        .getDefaultSensor(Sensor.TYPE_ACCELEROMETER),
                                SensorManager.SENSOR_DELAY_NORMAL);

                Log.i(TAG, "PersistService.RegisterListener");
            }
        }

    }
//--------------------------------------------------------------------------------


}


