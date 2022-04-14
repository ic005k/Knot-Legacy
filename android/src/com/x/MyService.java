package com.x;

import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
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


public class MyService extends Service {

    private static final String TAG = "MyService";
    private static final String ID = "channel_1";
    private static final String NAME = "F_SERVICE";

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

        if (Build.VERSION.SDK_INT >= 26) {
            setForeground();
        } else {

            Intent notificationIntent = new Intent(this, MyActivity.class);

            PendingIntent pendingIntent = PendingIntent.getActivity(this, 0,
                    notificationIntent, 0);

            Notification notification = new NotificationCompat.Builder(this)
                    .setSmallIcon(R.drawable.icon)
                    .setLargeIcon(BitmapFactory.decodeResource(getResources(), R.drawable.icon))
                    .setContentTitle("Xcounter")
                    .setContentText("Running...")
                    .setContentIntent(pendingIntent).build();

            startForeground(1337, notification);

        }
    }

    //服务在每次启动的时候调用的方法 如果某些行为在服务已启动的时候就执行，可以把处理逻辑写在这个方法里面
    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {

        Log.d("MyService", "onStartCommand()-------");

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
        startForeground(1, notification);
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
                NotificationChannel notificationChannel = new NotificationChannel("XCounter", "XCounter Notifier", importance);
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
}
