package com.x;

import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.IBinder;
import android.util.Log;
import android.widget.Toast;

import android.app.Notification;
import android.app.PendingIntent;
import android.R.mipmap;
import android.graphics.BitmapFactory;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.graphics.Color;
import android.graphics.BitmapFactory;
import android.app.NotificationChannel;
import android.support.v4.app.NotificationCompat;


public class MyService extends Service {

    private static final String TAG = "MyService";

    @Override
    public IBinder onBind(Intent arg0) {
        // TODO Auto-generated method stub
        Log.i(TAG, "Service on bind");//服务被绑定
        return null;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        Log.i(TAG, "Service on create");//服务被创建

        Intent notificationIntent = new Intent(this, MyActivity.class);

        PendingIntent pendingIntent = PendingIntent.getActivity(this, 0,
                        notificationIntent, 0);

        Notification notification = new NotificationCompat.Builder(this)
                        .setSmallIcon(R.drawable.icon)
                        .setContentTitle("My Awesome App")
                        .setContentText("Doing some work...")
                        .setContentIntent(pendingIntent).build();

        startForeground(1337, notification);

    }

    //服务在每次启动的时候调用的方法 如果某些行为在服务已启动的时候就执行，可以把处理逻辑写在这个方法里面
    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {

        Log.d("MyService","onStartCommand()-------");

        return super.onStartCommand(intent, flags, startId);
    }

    //服务销毁的时候调用的方法 可以回收部分不再使用的资源
    @Override
    public void onDestroy() {
        Log.d("MyService","onDestroy()-------");
        super.onDestroy();

    }
}
