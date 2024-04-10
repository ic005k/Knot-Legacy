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
import android.net.Uri;

import java.util.List;
import java.util.ArrayList;

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
import android.app.ActivityManager;
import android.content.pm.ApplicationInfo;

import java.util.logging.Logger;

import android.widget.Toast;

public class ShareReceiveActivity extends Activity {
    private TextView tv;
    public static String strData;

    private InternalConfigure internalConfigure;

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
        System.out.println("type=" + type);
        //设置接收类型为文本
        if (Intent.ACTION_SEND.equals(action) && type != null) {
            if ("text/plain".equals(type)) {
                handlerText(intent);
                goReceiveString();
            } else if (type.startsWith("image/")) {
                readFileFromShare("/storage/emulated/0/.Knot/receive_share_pic.png");
                CallJavaNotify_5();
                MyActivity.setMax();
            } else if (Intent.ACTION_SEND_MULTIPLE.equals(action) && type != null) {
                if (type.startsWith("image/")) {
                    dealMultiplePicStream(intent);
                    CallJavaNotify_5();
                    MyActivity.setMax();
                }
            } else if (type.startsWith("*/*")) {
                readFileFromShare("/storage/emulated/0/.Knot/receive_share_file.txt");
                CallJavaNotify_5();
                MyActivity.setMax();
            }
        }


        ShareReceiveActivity.this.finish();
    }

    //该方法用于获取intent所包含的文本信息，并显示到APP的Activity界面上
    public void handlerText(Intent intent) {
        strData = intent.getStringExtra(Intent.EXTRA_TEXT);
        String title = intent.getStringExtra(Intent.EXTRA_TITLE);
        tv.setText(strData);

    }

    void dealPicStream(Intent intent) {
        Uri uri = intent.getParcelableExtra(Intent.EXTRA_STREAM);
    }

    void dealMultiplePicStream(Intent intent) {
        ArrayList<Uri> arrayList = intent.getParcelableArrayListExtra(intent.EXTRA_STREAM);
    }

    void goReceiveString() {
        System.out.println("strData=" + strData);

        // Save receive data
        String file2 = "/storage/emulated/0/.Knot/myshare.ini";
        internalConfigure = new InternalConfigure(this);
        Properties myPro = new Properties();
        myPro.setProperty("receiveData", strData);

        boolean isRun = isAppRun("com.x");

        if (!isRun) {

            myPro.setProperty("shareDone", "false");
            try {
                internalConfigure.saveFile(file2, myPro);
            } catch (Exception e) {
                System.err.println("Error : save myshare.ini");
                e.printStackTrace();
            }

            Toast.makeText(this, "The Knot is not open, it will be opened for you at this time, please wait...", Toast.LENGTH_LONG).show();
            // reopen app
            PackageManager packageManager = getPackageManager();
            Intent it = packageManager.getLaunchIntentForPackage("com.x");
            startActivity(it);

        } else {
            myPro.setProperty("shareDone", "true");
            try {
                internalConfigure.saveFile(file2, myPro);
            } catch (Exception e) {
                System.err.println("Error : save myshare.ini");
                e.printStackTrace();
            }
            CallJavaNotify_5();
            MyActivity.setMax();
        }

    }

    @Override
    public void onBackPressed() {
        super.onBackPressed();

        ShareReceiveActivity.this.finish();
    }

    @Override
    protected void onDestroy() {
        System.out.println("onDestroy...");

        //android.os.Process.killProcess(android.os.Process.myPid());
        super.onDestroy();

    }

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

    private void startLocalApp(String packageNameTarget) {
        Log.i("Wmx logs::", "-----------------------开始启动第三方 APP=" + packageNameTarget);

        PackageManager packageManager = getPackageManager();
        Intent intent = packageManager.getLaunchIntentForPackage(packageNameTarget);
        intent.addCategory(Intent.CATEGORY_LAUNCHER);
        intent.setFlags(Intent.FLAG_ACTIVITY_RESET_TASK_IF_NEEDED | Intent.FLAG_ACTIVITY_NEW_TASK);

        /**android.intent.action.MAIN：打开另一程序
         */
        intent.setAction("android.intent.action.MAIN");
        /**
         * FLAG_ACTIVITY_SINGLE_TOP:
         * 如果当前栈顶的activity就是要启动的activity,则不会再启动一个新的activity
         */
        intent.setFlags(Intent.FLAG_ACTIVITY_SINGLE_TOP);
        startActivity(intent);

    }

    public class InternalConfigure {
        private final Context context;
        private Properties properties;

        public InternalConfigure(Context context) {
            super();
            this.context = context;
        }

        /**
         * 保存文件filename为文件名，filecontent为存入的文件内容
         * 例:configureActivity.saveFiletoSD("text.ini","");
         */
        public void saveFile(String filename, Properties properties) throws Exception {
            //设置Context.MODE_PRIVATE表示每次调用该方法会覆盖原来的文件数据
            FileOutputStream fileOutputStream;// = context.openFileOutput(filename, Context.MODE_PRIVATE);
            File file = new File(filename);
            fileOutputStream = new FileOutputStream(file);
            //通过properties.stringPropertyNames()获得所有key的集合Set，里面是String对象
            for (String key : properties.stringPropertyNames()) {
                String s = key + " = " + properties.getProperty(key) + "\n";
                System.out.println(s);
                fileOutputStream.write(s.getBytes());
            }
            fileOutputStream.close();
        }

        /**
         * 读取文件
         */
        public void readFrom(String filename) throws Exception {
            properties = new Properties();

            FileInputStream fileInputStream;// = context.openFileInput(filename);

            File file = new File(filename);
            fileInputStream = new FileInputStream(file);

            InputStreamReader reader = new InputStreamReader(fileInputStream, "UTF-8");
            BufferedReader br = new BufferedReader(reader);

            //debug:
            //String line;
            //while ((line = br.readLine()) != null) {
            //    System.out.println(line);
            //}

            properties.load(br);

            br.close();
            reader.close();
            fileInputStream.close();
        }

        /**
         * 返回指定key对应的value
         */
        public String getIniKey(String key) {
            if (properties.containsKey(key) == false) {
                return null;
            }
            return String.valueOf(properties.get(key));
        }
    }

    public boolean isAppRun(String pName) {
        Context context = MyActivity.context;

        boolean isRun = false;
        int uid = getPackageUid(context, pName);
        if (uid > 0) {
            boolean rstA = isAppRunning(context, pName);
            boolean rstB = isProcessRunning(context, uid);
            //if (rstA || rstB) {
            if (rstB) {
                //指定包名的程序正在运行中
                isRun = true;
            } else {
                //指定包名的程序未在运行中
                isRun = false;
            }
        } else {
            //应用未安装
        }
        return isRun;
    }


    /**
     * 方法描述：判断某一应用是否正在运行
     * Created by cafeting on 2017/2/4.
     *
     * @param context     上下文
     * @param packageName 应用的包名
     * @return true 表示正在运行，false 表示没有运行
     */
    public static boolean isAppRunning(Context context, String packageName) {
        ActivityManager am = (ActivityManager) context.getSystemService(Context.ACTIVITY_SERVICE);
        List<ActivityManager.RunningTaskInfo> list = am.getRunningTasks(100);
        if (list.size() <= 0) {
            return false;
        }
        for (ActivityManager.RunningTaskInfo info : list) {
            if (info.baseActivity.getPackageName().equals(packageName)) {
                return true;
            }
        }
        return false;
    }

    //获取已安装应用的 uid，-1 表示未安装此应用或程序异常
    public static int getPackageUid(Context context, String packageName) {
        try {
            ApplicationInfo applicationInfo = context.getPackageManager().getApplicationInfo(packageName, 0);
            if (applicationInfo != null) {
                Log.e("share", String.valueOf(applicationInfo.uid));
                return applicationInfo.uid;
            }
        } catch (Exception e) {
            return -1;
        }
        return -1;
    }

    /**
     * 判断某一 uid 的程序是否有正在运行的进程，即是否存活
     * Created by cafeting on 2017/2/4.
     *
     * @param context 上下文
     * @param uid     已安装应用的 uid
     * @return true 表示正在运行，false 表示没有运行
     */
    public static boolean isProcessRunning(Context context, int uid) {
        ActivityManager am = (ActivityManager) context.getSystemService(Context.ACTIVITY_SERVICE);
        List<ActivityManager.RunningServiceInfo> runningServiceInfos = am.getRunningServices(200);
        if (runningServiceInfos.size() > 0) {
            for (ActivityManager.RunningServiceInfo appProcess : runningServiceInfos) {
                if (uid == appProcess.uid) {
                    return true;
                }
            }
        }
        return false;
    }

    //读取分享的文件并把文件导入到私有目录
    public boolean readFileFromShare(String fileName) {
        Intent intent = getIntent();
        String type = intent.getType();
        if (intent.getAction().equalsIgnoreCase(Intent.ACTION_SEND)) {
            Uri uri = intent.getParcelableExtra(Intent.EXTRA_STREAM);
            if (uri != null) {
                try {
                    // 之前的目录存储位置备注：getExternalFilesDir(null).getPath() + File.separator = /storage/emulated/0/Android/data/com.x/files
                    File outFile = new File(fileName);
                    InputStream inputStream = getContentResolver().openInputStream(uri);
                    FileOutputStream fos = new FileOutputStream(outFile);
                    byte[] buf = new byte[1024];
                    int readCount = 0;
                    while ((readCount = inputStream.read(buf)) != -1) {
                        fos.write(buf, 0, readCount);
                    }
                    fos.flush();
                    inputStream.close();
                    fos.close();
                    return true;
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
        return false;
    }

}
