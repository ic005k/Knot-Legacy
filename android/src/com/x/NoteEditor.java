package com.x;

import com.x.MyActivity;

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
import android.widget.EditText;

import java.io.OutputStreamWriter;
import java.io.BufferedWriter;
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

import java.io.File;
import java.io.IOException;
import java.io.RandomAccessFile;

public class NoteEditor extends Activity implements View.OnClickListener, Application.ActivityLifecycleCallbacks {

    private MediaPlayer mediaPlayer;
    private static int curVol;
    private static String strInfo = "Todo|There are currently timed tasks pending.|0|Close";
    private static String strEnInfo = "Todo|There are currently timed tasks pending.|0|Close";
    private String strMute = "false";
    private boolean isRefreshAlarm = true;
    private AudioManager mAudioManager;
    private InternalConfigure internalConfigure;

    private Button btn_cancel;
    private EditText text_info;
    private static boolean zh_cn;

    private static Context context;
    private static NoteEditor m_instance;

    public static Context getContext() {
        return context;
    }

    public native static void CallJavaNotify_0();

    public native static void CallJavaNotify_1();

    public native static void CallJavaNotify_2();

    public native static void CallJavaNotify_3();

    public native static void CallJavaNotify_4();

    public native static void CallJavaNotify_5();

    public native static void CallJavaNotify_6();

    private static boolean isGoBackKnot = false;

    public static boolean isZh(Context context) {
        Locale locale = context.getResources().getConfiguration().locale;
        String language = locale.getLanguage();
        if (language.endsWith("zh"))
            zh_cn = true;
        else
            zh_cn = false;

        return zh_cn;
    }

    public static int setInfoText(String str) {
        strInfo = str;
        System.out.println("InfoText" + strInfo);
        return 1;
    }


    private void bindViews(String str) {
        text_info = (EditText) findViewById(R.id.text_info);
        text_info.setText(str);

        btn_cancel = (Button) findViewById(R.id.btn_cancel);
        
        if (zh_cn)
            btn_cancel.setText("保存");
        else
            btn_cancel.setText("Save");
        btn_cancel.setOnClickListener(this);

    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btn_cancel:
                // NoteEditor.this.finish();
                // btn_cancel.setVisibility(View.GONE);


                String mContent = text_info.getText().toString();
                String mPath = "/storage/emulated/0/.Knot/";
                writeTxtToFile(mContent, mPath, "note_text.txt");


                CallJavaNotify_6();
                onBackPressed();
                break;
        }
    }

    private void AnimationWhenClosed() {
        // 淡出效果
        //overridePendingTransition(android.R.anim.fade_in, android.R.anim.fade_out);

        // 或者使用底部滑出效果(自定义文件exit_anim.xml)
        overridePendingTransition(0, R.anim.exit_anim);
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);


        context = getApplicationContext();
        //this.getWindow().setWindowAnimations(R.style.WindowAnim);
        isZh(context);
        m_instance = this;
        registerReceiver(mHomeKeyEvent, new IntentFilter(Intent.ACTION_CLOSE_SYSTEM_DIALOGS));
        Application application = this.getApplication();
        application.registerActivityLifecycleCallbacks(this);


        String filename = "/storage/emulated/0/.Knot/note_text.ini";
        internalConfigure = new InternalConfigure(this);
        try {
            internalConfigure.readFrom(filename);
        } catch (Exception e) {
            System.err.println("Error : reading note_text.ini");
            e.printStackTrace();
        }
        if (fileIsExists(filename)) {
            strInfo = internalConfigure.getIniKey("text");
        }
        System.out.println("Info Text: " + strInfo);

        //去除title(App Name)
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        //去掉Activity上面的状态栏(Show Time...)
        //getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        this.setStatusBarColor("#F3F3F3");  //灰
        getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_LIGHT_STATUS_BAR);


        setContentView(R.layout.noteeditor);
        bindViews(strInfo);


        // HomeKey
        registerReceiver(mHomeKeyEvent, new IntentFilter(Intent.ACTION_CLOSE_SYSTEM_DIALOGS));

    }

    private BroadcastReceiver mHomeKeyEvent = new BroadcastReceiver() {
        String SYSTEM_REASON = "reason";
        String SYSTEM_HOME_KEY = "homekey";
        String SYSTEM_HOME_KEY_LONG = "recentapps";

        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if (action.equals(Intent.ACTION_CLOSE_SYSTEM_DIALOGS)) {
                String reason = intent.getStringExtra(SYSTEM_REASON);
                if (TextUtils.equals(reason, SYSTEM_HOME_KEY)) {
                    // 表示按了home键,程序直接进入到后台

                    System.out.println("NoteEditor HOME键被按下...");
                    onBackPressed();
                } else if (TextUtils.equals(reason, SYSTEM_HOME_KEY_LONG)) {
                    // 表示长按home键,显示最近使用的程序
                    System.out.println("NoteEditor 长按HOME键...");
                    onBackPressed();
                }
            }
        }
    };

    @Override
    public void onPause() {
        System.out.println("NoteEditor onPause...");
        super.onPause();

    }

    @Override
    public void onStop() {
        System.out.println("NoteEditor onStop...");

        super.onStop();

    }

    @Override
    public void onBackPressed() {
        super.onBackPressed();

        AnimationWhenClosed();
    }

    @Override
    protected void onDestroy() {


        unregisterReceiver(mHomeKeyEvent);
        MyService.clearNotify();


        super.onDestroy();

        System.out.println("NoteEditor onDestroy...");

    }

    public static void close() {

        if (m_instance != null) {
            m_instance.finish();
        }
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

    //fileName 为文件名称 返回true为存在
    public boolean fileIsExists(String fileName) {
        try {
            File f = new File(fileName);
            if (f.exists()) {
                Log.i("测试", "文件存在：" + fileName);
                return true;
            } else {
                Log.i("测试", "没有这个文件: " + fileName);
                return false;
            }
        } catch (Exception e) {
            Log.i("测试", "出现异常！");
            e.printStackTrace();
            return false;
        }
    }

    public String readText(String filename) throws Exception {
        FileInputStream fileInputStream;
        File file = new File(filename);
        fileInputStream = new FileInputStream(file);

        InputStreamReader reader = new InputStreamReader(fileInputStream, "UTF-8");
        BufferedReader br = new BufferedReader(reader);
        String line = br.readLine();
        //while ((line = br.readLine()) != null) {
        //    System.out.println(line);
        //}

        br.close();
        reader.close();
        fileInputStream.close();

        return line;
    }


    private void setStatusBarColor(String color) {
        // 需要安卓版本大于5.0以上
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            getWindow().addFlags(WindowManager.LayoutParams.FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS);
            getWindow().setStatusBarColor(Color.parseColor(color));
        }
    }

    @Override
    public void onActivityCreated(Activity activity, Bundle savedInstanceState) {

    }

    @Override
    public void onActivityStarted(Activity activity) {

    }

    @Override
    public void onActivityResumed(Activity activity) {

    }

    @Override
    public void onActivityPaused(Activity activity) {

    }

    @Override
    public void onActivityStopped(Activity activity) {
        System.out.println("NoteEditor onActivityStopped...");
    }

    @Override
    public void onActivitySaveInstanceState(Activity activity, Bundle outState) {

    }

    @Override
    public void onActivityDestroyed(Activity activity) {

    }

    private void save(String s, String file) {
        OutputStream out = null;
        BufferedWriter writer = null;
        try {
            out = openFileOutput(file, MODE_PRIVATE);
            writer = new BufferedWriter(new OutputStreamWriter(out));
            writer.write(s);
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            try {
                if (writer != null) {
                    writer.close();
                }
            } catch (Exception e) {
                e.printStackTrace();
            }

        }
    }


    // 将字符串写入到文本文件中
    public void writeTxtToFile(String content, String filePath, String fileName) {
        //生成文件夹之后，再生成文件，不然会出错
        makeFile(filePath, fileName);
        String mFilePath = filePath + fileName;
        // 每次写入时，都换行写
        String mContent = content + "\r\n";
        try {
            File file = new File(mFilePath);
            if (!file.exists()) {
                System.out.println("创建文件: " + mFilePath);
                file.getParentFile().mkdirs();
                file.createNewFile();
            }
            RandomAccessFile mRandomAccessFile = new RandomAccessFile(file, "rwd");
            mRandomAccessFile.seek(file.length());
            mRandomAccessFile.write(mContent.getBytes());
            mRandomAccessFile.close();
        } catch (IOException e) {
            System.out.println("写入错误: " + e.toString());
        }
    }

    //生成文件
    public File makeFile(String filePath, String fileName) {
        File file = null;
        makeDirectory(filePath);
        try {
            file = new File(filePath + fileName);
            if (!file.exists()) {
                file.createNewFile();
            }
        } catch (IOException e) {
            System.out.println("生成文件错误: " + e.toString());
        }
        return file;
    }

    //生成文件夹
    public void makeDirectory(String filePath) {
        File file = null;
        try {
            file = new File(filePath);
            if (!file.exists()) {
                file.mkdir();
            }
        } catch (Exception e) {
            System.out.println("生成文件夹错误: " + e.toString());
        }
    }


}
