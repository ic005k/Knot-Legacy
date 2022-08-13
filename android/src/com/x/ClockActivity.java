package com.x;

import org.qtproject.qt5.android.bindings.QtActivity;

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
import android.view.WindowManager;

//import android.support.v7.app.AppCompatActivity;
//import androidx.appcompat.app.AppCompatActivity;

import java.io.BufferedReader;
import java.io.InputStream;
import java.io.FileOutputStream;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.util.HashMap;
import java.util.Map;
import java.util.Properties;
import java.io.IOException;
import java.io.File;
import java.text.SimpleDateFormat;
import java.util.Date;

import android.media.AudioManager;

public class ClockActivity extends Activity {

    private MediaPlayer mediaPlayer;
    private static String strInfo = "Todo|There are currently timed tasks pending.|0|Close";
    private AudioManager mAudioManager;

    private static Context context;
    public static Context getContext() {
        return context;
    }

    public native static void CallJavaNotify_1();
    public native static void CallJavaNotify_2();
    public native static void CallJavaNotify_3();

    public static int setInfoText(String str) {
        strInfo = str;
        System.out.println("InfoText" + strInfo);
        return 1;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        context = getApplicationContext();

        //SimpleDateFormat formatter = new SimpleDateFormat("yyyy-MM-dd 'at' HH:mm:ss z");
        SimpleDateFormat formatter = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
        Date date = new Date(System.currentTimeMillis());
        String strCurDT0 = formatter.format(date);
        String strCurDT = " ( " + strCurDT0 + " ) ";

        //this.setStatusBarColor("#F3F3F3");  //灰

        MediaPlayer mediaPlayer = new MediaPlayer();

        mAudioManager = (AudioManager) context.getSystemService(Service.AUDIO_SERVICE);
        int curVol = getMediaVolume();
        int maxVol = getMediaMaxVolume();
        double vol =  maxVol*0.75;
        setMediaVolume((int)Math.round(vol));
        System.out.println("maxVol:  " + maxVol + "    setvol:  " + vol);
        try {
            mediaPlayer.setDataSource("/data/data/com.x/files/msg.mp3");
            mediaPlayer.prepare();
            mediaPlayer.start();
        } catch (Exception e) {
            e.printStackTrace();
        }

        String filename = "/data/data/com.x/files/msg.ini";
        if (!fileIsExists(filename)) {
            try {
                InternalConfigure internalConfigure = new InternalConfigure(this);
                internalConfigure.readFrom(filename);
                String strCount = internalConfigure.getIniKey("count");
                int count = Integer.parseInt(strCount);
                for (int i=0;i<count;i++)
                {
                    String str = internalConfigure.getIniKey("msg" + String.valueOf(i+1));
                    String[] arr1 = str.split("\\|");
                    String str1 = arr1[0];
                    //if(str1 == strCurDT0)
                    if(str.contains(strCurDT0))
                    {
                        strInfo = str;
                        break;
                    }

                    System.out.println("Read Ini: " + str);
                    System.out.println(str1 + "    " + strCurDT0);
                }

                //strInfo = readText(filename);
            } catch (Exception e) {
                System.err.println("Error : reading msg.ini");
                e.printStackTrace();
            }
        }

        System.out.println("Info Text: " + strInfo);

        String[] array = strInfo.split("\\|");
        String str1 = array[0];
        String str2 = array[1];
        String str3 = array[3];

        new AlertDialog.Builder(ClockActivity.this).setTitle(str1).setMessage(str2 + "\n\n\n" + strCurDT)
                .setPositiveButton(str3, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        mediaPlayer.stop();
                        setMediaVolume(curVol);
                        ClockActivity.this.finish();
                    }
                }).show();

        //CallJavaNotify_2();

        System.out.println("闹钟已开始+++++++++++++++++++++++");
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

    //获取最大多媒体音量
    public int getMediaMaxVolume() {
        return mAudioManager.getStreamMaxVolume(AudioManager.STREAM_MUSIC);
    }

    //获取当前多媒体音量
    public int getMediaVolume() {
        return mAudioManager.getStreamVolume(AudioManager.STREAM_MUSIC);
    }

    // 设置多媒体音量
    public void setMediaVolume(int volume) {
        mAudioManager.setStreamVolume(AudioManager.STREAM_MUSIC, //音量类型
                volume,
                AudioManager.FLAG_PLAY_SOUND
                        | AudioManager.FLAG_SHOW_UI);
    }

    private void setStatusBarColor(String color) {
        // 需要安卓版本大于5.0以上
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            getWindow().addFlags(WindowManager.LayoutParams.FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS);
            getWindow().setStatusBarColor(Color.parseColor(color));
        }
    }

}
