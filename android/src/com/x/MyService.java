package com.x;

import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.IBinder;
import android.util.Log;
import android.os.Build;

import android.app.Notification;
import android.app.PendingIntent;
import android.graphics.BitmapFactory;
import android.app.Notification;
import android.app.NotificationManager;
import android.graphics.Color;
import android.app.NotificationChannel;
import android.support.v4.app.NotificationCompat;
import android.annotation.TargetApi;

import java.sql.Time;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Timer;
import java.util.TimerTask;


public class MyService extends Service {

    private static final String TAG = "MyService";
    private static final String ID = "channel_1";
    private static final String NAME = "F_SERVICE";

    public native static void CallJavaNotify_1();

    public native static void CallJavaNotify_2();

    public native static void CallJavaNotify_3();


    //private static SimpleDateFormat format = new SimpleDateFormat("yyyy/MM/dd HH:mm:ss");
    private static SimpleDateFormat format = new SimpleDateFormat("HH:mm:ss");
    public static Timer timer;
    public static Timer timerAlarm;
    public static int sleep = 0;

    public static int startTimerAlarm() {
        stopTimerAlarm();
        System.out.println("startTimerAlarm+++++++++++++++++++++++");
        timerAlarm = new Timer();
        timerAlarm.schedule(new TimerTask() {
            @Override
            public void run() {
                CallJavaNotify_3();
            }
        }, 0, 60000);
        return 1;
    }

    public static int stopTimerAlarm() {
        if (timerAlarm != null) {
            timerAlarm.cancel();
            timerAlarm.purge();
            System.out.println("stopTimerAlarm+++++++++++++++++++++++");
        }
        return 1;
    }

    public static int startTimer() {
        System.out.println("startTimer+++++++++++++++++++++++");

        timer = new Timer();
        timer.schedule(new TimerTask() {
            @Override
            public void run() {
                if (MyActivity.isStepCounter == 0)
                    CallJavaNotify_1();
                if (MyActivity.isStepCounter == 1)
                    CallJavaNotify_2();


            }
        }, 0, sleep);
        return 1;
    }


    public static int stopTimer() {
        if (timer != null) {
            timer.cancel();
            timer.purge();
            System.out.println("stopTimer+++++++++++++++++++++++");
        }
        return 1;
    }

    public static int setSleep1() {
        sleep = 200;
        stopTimer();
        startTimer();
        System.out.println("setSleep1+++++++++++++++++++++++");
        return 1;
    }

    public static int setSleep2() {
        sleep = 10;
        stopTimer();
        startTimer();
        System.out.println("setSleep2+++++++++++++++++++++++");
        return 1;
    }

    public static int setSleep3() {
        sleep = 5000;
        stopTimer();
        startTimer();
        System.out.println("setSleep3+++++++++++++++++++++++");
        return 1;
    }


    @Override
    public IBinder onBind(Intent arg0) {
        // Auto-generated method stub
        Log.i(TAG, "Service on bind");//服务被绑定
        return null;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        Log.i(TAG, "Service on create");//服务被创建

    }

    //服务在每次启动的时候调用的方法 如果某些行为在服务已启动的时候就执行，可以把处理逻辑写在这个方法里面
    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {

        Log.d("MyService", "onStartCommand()-------");

        if (Build.VERSION.SDK_INT >= 26) {
            setForeground();
        } else {
            Intent notificationIntent = new Intent(this, MyActivity.class);
            PendingIntent pendingIntent = PendingIntent.getActivity(this, 0,
                    notificationIntent, 0);
            Notification notification = new NotificationCompat.Builder(this)
                    .setSmallIcon(R.drawable.icon)
                    .setLargeIcon(BitmapFactory.decodeResource(getResources(), R.drawable.icon))
                    .setContentTitle("Knot")
                    .setContentText("Running...")
                    .setContentIntent(pendingIntent).build();
            startForeground(1337, notification);
        }

        return super.onStartCommand(intent, flags, startId);
    }

    //服务销毁的时候调用的方法 可以回收部分不再使用的资源
    @Override
    public void onDestroy() {
        Log.d("MyService", "onDestroy()-------");
        super.onDestroy();

    }

    @TargetApi(26)
    private void setForeground() {
        Context context;
        NotificationManager manager = (NotificationManager) getSystemService(NOTIFICATION_SERVICE);
        NotificationChannel channel = new NotificationChannel(ID, NAME, NotificationManager.IMPORTANCE_LOW);
        manager.createNotificationChannel(channel);
        Notification notification = new Notification.Builder(this, ID)
                .setContentTitle("Powerful Counter")
                .setContentText("Running...")
                .setSmallIcon(R.drawable.icon)
                .setLargeIcon(BitmapFactory.decodeResource(getResources(), R.drawable.icon))
                .build();
        startForeground(1337, notification);
    }

    //----------------------------------------------
    private static NotificationManager m_notificationManager;
    private static Notification.Builder m_builder;

    public static void notify(Context context, String message) {
        try {
            m_notificationManager = (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);

            if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.O) {
                //int importance = NotificationManager.IMPORTANCE_DEFAULT;
                int importance = NotificationManager.IMPORTANCE_LOW; //这个低频道不包含任何声音，达到静音的效果
                NotificationChannel notificationChannel = new NotificationChannel("Knot", "Knot Notifier Steps", importance);
                //notificationChannel.setSound(null, null);//设置频道静音
                m_notificationManager.createNotificationChannel(notificationChannel);

                m_builder = new Notification.Builder(context, notificationChannel.getId());
                //m_builder.setOnlyAlertOnce(true);
            } else {
                m_builder = new Notification.Builder(context);
            }

            m_builder.setContentTitle("Pedometer")
                    .setContentText(message)
                    .setSmallIcon(R.drawable.icon)
                    .setColor(Color.GREEN)
                    .setAutoCancel(true);
            //.setDefaults(Notification.DEFAULT_LIGHTS)
            //.setSmallIcon(R.drawable.icon)
            //.setLargeIcon(BitmapFactory.decodeResource(context.getResources(), R.drawable.icon))

            m_notificationManager.notify(0, m_builder.build());


            //startForeground(1,m_builder.build());
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    //----------------------------------------------
    private static NotificationManager m_notificationManagerAlarm;
    private static Notification.Builder m_builderAlarm;
    public static void notifyAlarm(Context context, String message) {
        try {
            m_notificationManagerAlarm = (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);

            if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.O) {
                int importance = NotificationManager.IMPORTANCE_DEFAULT;
                NotificationChannel notificationChannel = new NotificationChannel("Knot Alarm", "Knot Notifier Alarm", importance);
                m_notificationManagerAlarm.createNotificationChannel(notificationChannel);

                m_builderAlarm = new Notification.Builder(context, notificationChannel.getId());
                //m_builder.setOnlyAlertOnce(true);
            } else {
                m_builderAlarm = new Notification.Builder(context);
            }

            m_builderAlarm.setContentTitle("Todo")
                    .setContentText(message)
                    .setSmallIcon(R.drawable.icon)
                    .setColor(Color.GREEN)
                    .setDefaults(Notification.DEFAULT_LIGHTS)
                    .setAutoCancel(true);
            //.setSmallIcon(R.drawable.icon)
            //.setLargeIcon(BitmapFactory.decodeResource(context.getResources(), R.drawable.icon))

            m_notificationManagerAlarm.notify(10, m_builderAlarm.build());


            //startForeground(1,m_builder.build());
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
