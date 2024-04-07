package com.x;

import com.x.MyActivity;

import android.content.ComponentName;
import android.content.Intent;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

import java.util.List;

import android.content.IntentFilter;
import android.content.Intent;
import android.content.BroadcastReceiver;
import android.app.PendingIntent;
import android.text.TextUtils;
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
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.view.WindowManager;
import android.view.Window;

import java.io.BufferedReader;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.FileOutputStream;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.util.HashMap;
import java.util.Locale;
import java.util.Map;
import java.util.Properties;
import java.io.IOException;
import java.io.File;
import java.text.SimpleDateFormat;
import java.util.Date;

import android.os.Handler;
import android.media.AudioManager;
import android.widget.TextView;

import java.util.Locale;

import android.app.Application;

public class ShareReceiveActivity extends Activity {
    private TextView tv;
    static public String strData;

    public native static void CallJavaNotify_0();

    public native static void CallJavaNotify_1();

    public native static void CallJavaNotify_2();

    public native static void CallJavaNotify_3();

    public native static void CallJavaNotify_4();

    public native static void CallJavaNotify_5();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_share_receive);
        tv = (TextView) findViewById(R.id.text_info);


        //获取intent
        Intent intent = getIntent();
        String action = intent.getAction();
        String type = intent.getType();
        //设置接收类型为文本
        if (Intent.ACTION_SEND.equals(action) && type != null) {
            if ("text/plain".equals(type)) {
                handlerText(intent);
            }
        }

        //显示一个警报框，目前已弃用，采用全屏幕显示
       /* new AlertDialog.Builder(ShareReceiveActivity.this).setTitle("Knot").setMessage(strData)
                .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {

                        ShareReceiveActivity.this.finish();
                    }
                }).show();*/

        System.out.println("strData=" + strData);
        CallJavaNotify_5();
        MyActivity.setMax();
        ShareReceiveActivity.this.finish();

    }

    //该方法用于获取intent所包含的文本信息，并显示到APP的Activity界面上
    public void handlerText(Intent intent) {
        strData = intent.getStringExtra(Intent.EXTRA_TEXT);
        tv.setText(strData);
        //MyActivity.ReOpen = true;
    }

    @Override
    public void onBackPressed() {
        super.onBackPressed();

        ShareReceiveActivity.this.finish();
    }

    @Override
    protected void onDestroy() {
        System.out.println("onDestroy...");
        //doStartApplicationWithPackageName("com.x");
        //android.os.Process.killProcess(android.os.Process.myPid());
        super.onDestroy();

    }

    //@Override
    //public void onActivityDestroyed(Activity activity) {
    //    }

    private void doStartApplicationWithPackageName(String packagename) {
        System.out.println("自启动开始...");
        // 通过包名获取此APP详细信息，包括Activities、services、versioncode、name等等
        PackageInfo packageinfo = null;
        try {
            packageinfo = getPackageManager().getPackageInfo(packagename, 0);
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
        }
        if (packageinfo == null) {
            return;
        }

        // 创建一个类别为CATEGORY_LAUNCHER的该包名的Intent
        Intent resolveIntent = new Intent(Intent.ACTION_MAIN, null);
        resolveIntent.addCategory(Intent.CATEGORY_LAUNCHER);
        resolveIntent.setPackage(packageinfo.packageName);

        // 通过getPackageManager()的queryIntentActivities方法遍历
        List<ResolveInfo> resolveinfoList = getPackageManager()
                .queryIntentActivities(resolveIntent, 0);

        ResolveInfo resolveinfo = resolveinfoList.iterator().next();
        if (resolveinfo != null) {
            // packagename = 参数packname
            String packageName = resolveinfo.activityInfo.packageName;
            // 这个就是我们要找的该APP的LAUNCHER的Activity[组织形式：packagename.mainActivityname]
            String className = resolveinfo.activityInfo.name;
            // LAUNCHER Intent
            Intent intent = new Intent(Intent.ACTION_MAIN);
            intent.addCategory(Intent.CATEGORY_LAUNCHER);

            // 设置ComponentName参数1:packagename参数2:MainActivity路径
            ComponentName cn = new ComponentName(packageName, className);

            intent.setComponent(cn);
            startActivity(intent);

            System.out.println("启动自己已完成...");
        }

        System.out.println("过程完成...");
    }


}
