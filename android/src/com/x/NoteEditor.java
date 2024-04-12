package com.x;

import com.x.MyActivity;

// 读写ini文件的三方开源库
import org.ini4j.Wini;

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
import android.text.Editable;
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
    private Button btnInsertImg;
    public EditText editNote;
    private static boolean zh_cn;
    private String currentMDFile;
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
    public native static void CallJavaNotify_7();

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
        editNote = (EditText) findViewById(R.id.editNote);
        editNote.setText(str);

        btn_cancel = (Button) findViewById(R.id.btn_cancel);
        btnInsertImg = (Button) findViewById(R.id.btnInsertImg);

        if (zh_cn) {
            btn_cancel.setText("保存");
            btnInsertImg.setText("插入图片");
        }
        else {
            btn_cancel.setText("Save");
            btnInsertImg.setText("Insert Img");
        }
        btn_cancel.setOnClickListener(this);
        btnInsertImg.setOnClickListener(this);

    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btn_cancel:
                // NoteEditor.this.finish();
                // btn_cancel.setVisibility(View.GONE);


                // save current text
                String mContent = editNote.getText().toString();
                String mPath = "/storage/emulated/0/.Knot/";
                writeTxtToFile(mContent, mPath, "note_text.txt");

                //save cursor pos
                String file2 = "/storage/emulated/0/.Knot/note_text.ini";
                int cpos = editNote.getSelectionStart();

                /*internalConfigure = new InternalConfigure(this);
                Properties myPro = new Properties();
                myPro.put(currentMDFile, String.valueOf(cpos));
                try {
                    internalConfigure.saveFile(file2, myPro);
                } catch (Exception e) {
                    System.err.println("Error : save note_text.ini");
                    e.printStackTrace();
                }*/

                try {
                    Wini ini = new Wini(new File(file2));

                    ini.put("cpos", currentMDFile, String.valueOf(cpos));

                    ini.store();
                } catch (IOException e) {
                    e.printStackTrace();
                }

                CallJavaNotify_6();
                onBackPressed();
                break;

            case R.id.btnInsertImg:

                CallJavaNotify_7();
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


        String filename = "/storage/emulated/0/.Knot/mymd.txt";
        strInfo = readFile(filename);


        //去除title(App Name)
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        //去掉Activity上面的状态栏(Show Time...)
        //getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        this.setStatusBarColor("#F3F3F3");  //灰
        getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_LIGHT_STATUS_BAR);


        setContentView(R.layout.noteeditor);
        bindViews(strInfo);

        //set cursor pos
        filename = "/storage/emulated/0/.Knot/note_text.ini";
        String s_cpos = null;

        /*internalConfigure = new InternalConfigure(this);
        try {
            internalConfigure.readFrom(filename);
        } catch (Exception e) {
            System.err.println("Error : reading note_text.ini");
            e.printStackTrace();
        }
         s_cpos = internalConfigure.getIniKey(currentMDFile);
         currentMDFile = internalConfigure.getIniKey("currentMDFile");
        */

        try {
            Wini ini = new Wini(new File(filename));
            currentMDFile = ini.get("cpos", "currentMDFile");
            s_cpos = ini.get("cpos", currentMDFile);

        } catch (IOException e) {
            e.printStackTrace();
        }

        if (fileIsExists(filename)) {
            int cpos;
            if (s_cpos == null)
                cpos = 0;
            else
                cpos = Integer.parseInt(s_cpos);
            if (cpos > strInfo.length())
                cpos = strInfo.length();
            editNote.setSelection(cpos);
        }


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
                    //onBackPressed();
                } else if (TextUtils.equals(reason, SYSTEM_HOME_KEY_LONG)) {
                    // 表示长按home键,显示最近使用的程序
                    System.out.println("NoteEditor 长按HOME键...");
                    //onBackPressed();
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

            // 每次都会生成一个新文件并抹掉除本次key之外的其他key数据
            //File file = new File(filename);
            fileOutputStream = new FileOutputStream(filename);

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

    public String readFile(String fileName) {
        StringBuilder sb = new StringBuilder("");
        try {
            File file = new File(fileName);
            //打开文件输入流
            FileInputStream inputStream = new FileInputStream(file);
            byte[] buffer = new byte[1024];
            int len = inputStream.read(buffer);
            //读取文件内容
            while (len > 0) {
                sb.append(new String(buffer, 0, len));
                //继续将数据放到buffer中
                len = inputStream.read(buffer);
            }
            //关闭输入流
            inputStream.close();
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        return sb.toString();
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

    public void appendNote(String str) {
        editNote.append(str);
    }

    public void insertNote(String str) {

        int index = editNote.getSelectionStart();//获取光标所在位置
        Editable edit = editNote.getEditableText();//获取EditText的文字
        if (index < 0 || index >= edit.length()) {
            edit.append(str);
        } else {
            edit.insert(index, str);//光标所在位置插入文字
        }
    }


}
