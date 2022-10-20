package com.x;

//Qt5

import org.qtproject.qt5.android.bindings.QtActivity;

import com.x.MyService;

import android.app.Application;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Application;
import android.content.DialogInterface;
import android.os.Bundle;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.content.ContentResolver;
import android.os.Looper;
import android.provider.DocumentsContract;
import android.provider.MediaStore;
import android.database.Cursor;
import android.content.ContentUris;
import android.content.UriMatcher;
import android.net.Uri;
import android.os.Environment;
import android.os.IBinder;
import android.os.Handler;
import android.os.Looper;
import android.app.PendingIntent;
import android.text.TextUtils;
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

import android.content.ContentUris;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.util.zip.ZipInputStream;
import java.io.BufferedOutputStream;
import java.io.BufferedInputStream;
import java.util.zip.ZipEntry;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.Iterator;
import java.util.List;
import java.util.zip.ZipException;
import java.util.zip.ZipFile;
import java.util.zip.ZipInputStream;
import java.util.Objects;
import java.util.zip.ZipOutputStream;
import java.util.Random;
import java.net.URLDecoder;
import java.nio.ByteBuffer;
import java.nio.MappedByteBuffer;
import java.nio.channels.FileChannel;

import android.app.AlarmManager;
import android.app.PendingIntent;
import android.widget.Toast;

import java.util.Calendar;

import android.app.AlertDialog;
import android.content.DialogInterface;

import android.support.v4.content.FileProvider;

public class MyActivity extends QtActivity implements Application.ActivityLifecycleCallbacks {

    private static MyActivity m_instance = null;
    private static SensorManager mSensorManager;
    public static int isStepCounter = -1;
    public static float stepCounts;
    private static WakeLock mWakeLock = null;
    private static PersistService mySerivece;
    private static final int DELAY = SensorManager.SENSOR_DELAY_NORMAL;

    private static AlarmManager alarmManager;
    private static PendingIntent pi;
    private static Intent intent;
    public static String strAlarmInfo;
    public static int alarmCount;
    public static boolean isScreenOff = false;

    public native static void CallJavaNotify_1();

    public native static void CallJavaNotify_2();

    public native static void CallJavaNotify_3();

    public native static void CallJavaNotify_4();

    public MyActivity() {
        //不用在此初始化，已采用新方法
        //m_instance = this;
    }

    //------------------------------------------------------------------------
    public static int startAlarm(String str) {
        // 特殊转义字符，必须加"\\"（“.”和“|”都是转义字符）
        String[] array = str.split("\\|");
        for (int i = 0; i < array.length; i++)
            System.out.println(array[i]);

        String strTime = array[0];
        String strText = array[1];
        String strTotalS = array[2];

        Calendar c = Calendar.getInstance();
        c.setTimeInMillis(System.currentTimeMillis());

        int ts = Integer.parseInt(strTotalS);
        c.add(Calendar.SECOND, ts);

        alarmManager.setExactAndAllowWhileIdle(AlarmManager.RTC_WAKEUP, c.getTimeInMillis(), pi);

        //定时精度不够
        //alarmManager.set(AlarmManager.RTC_WAKEUP, c.getTimeInMillis(), pi);

        Log.e("Alarm Manager", c.getTimeInMillis() + "");
        Log.e("Alarm Manager", str);

        System.out.println(ts);
        System.out.println("startAlarm+++++++++++++++++++++++");
        return 1;
    }

    public static int stopAlarm() {
        if (alarmManager != null) {
            if (pi != null) {
                alarmManager.cancel(pi);
            }

            System.out.println("stopAlarm+++++++++++++++++++++++" + alarmCount);
        }
        alarmCount = 0;

        return 1;
    }

    //------------------------------------------------------------------------
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
            //透明状态栏
            getWindow().addFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS);
            // 状态栏字体设置为深色，SYSTEM_UI_FLAG_LIGHT_STATUS_BAR 为SDK23增加
            getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN | View.SYSTEM_UI_FLAG_LIGHT_STATUS_BAR);
            // 部分机型的statusbar会有半透明的黑色背景
            getWindow().addFlags(WindowManager.LayoutParams.FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS);
            getWindow().clearFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS);
            getWindow().setStatusBarColor(Color.TRANSPARENT);// SDK21
        }
    }

    // 非全透,带颜色的状态栏,需要指定颜色（目前采用）
    private void setStatusBarColor(String color) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            // 需要安卓版本大于5.0以上
            getWindow().addFlags(WindowManager.LayoutParams.FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS);
            getWindow().setStatusBarColor(Color.parseColor(color));
        }
    }

    //----------------------------------------------------------------------

    private Sensor countSensor;

    public void initStepSensor() {
        if (countSensor != null) {
            if (mSensorManager != null) {
                mSensorManager.unregisterListener(mySerivece);
                mSensorManager
                        .registerListener(
                                mySerivece,
                                mSensorManager
                                        .getDefaultSensor(Sensor.TYPE_STEP_COUNTER),
                                SensorManager.SENSOR_DELAY_NORMAL);
            }
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
    private ScreenStatusReceiver mScreenStatusReceiver = null;

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
                isScreenOff = false;
                Log.w("Knot", "屏幕亮了");

            } else if (SCREEN_OFF.equals(intent.getAction())) {
                if (isStepCounter == 1) {
                    if (mySerivece != null) {
                        mSensorManager.unregisterListener(mySerivece);

                    }
                }
                isScreenOff = true;
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

    //--------------------------------------------------------------------------------------------------
    // 目前正在使用，经过改良，目前能解压所有的epub文件
    public static void Unzip(String zipFile, String targetDir) {
        Log.i(TAG, zipFile);
        Log.i(TAG, targetDir);

        int BUFFER = 1024 * 1024; //这里缓冲区我们使用4KB，
        String strEntry; //保存每个zip的条目名称
        try {
            BufferedOutputStream dest = null; //缓冲输出流
            InputStream fis = new FileInputStream(zipFile);
            ZipInputStream zis = new ZipInputStream(new BufferedInputStream(fis));
            ZipEntry entry; //每个zip条目的实例
            while ((entry = zis.getNextEntry()) != null) {
                try {
                    // 先创建目录，否则有些文件没法解压，比如根目录里面的文件
                    if (entry.isDirectory()) {
                        File fmd = new File(targetDir + entry.getName());
                        fmd.mkdirs();
                        continue;
                    }
                    int count;
                    byte data[] = new byte[BUFFER];
                    strEntry = entry.getName();
                    File entryFile = new File(targetDir + strEntry);
                    Log.i(TAG, targetDir + strEntry);
                    File entryDir = new File(entryFile.getParent());
                    Log.i(TAG, " entryDir: " + entryFile.getParent());
                    if (!entryDir.exists()) {
                        entryDir.mkdirs();
                    }
                    FileOutputStream fos = new FileOutputStream(entryFile);
                    dest = new BufferedOutputStream(fos, BUFFER);
                    while ((count = zis.read(data, 0, BUFFER)) != -1) {

                        dest.write(data, 0, count);
                    }
                    dest.flush();
                    dest.close();
                } catch (Exception ex) {
                    ex.printStackTrace();
                }
            }
            zis.close();
            Log.i(TAG, "unzip end...");
        } catch (Exception cwj) {
            cwj.printStackTrace();
        }
    }

    /**
     * 读取文件内容并压缩，既支持文件也支持文件夹
     *
     * @param filePath 文件路径
     */
    private static void compressFileToZip(String filePath,
                                          String zipFilePath) {
        try (ZipOutputStream zos = new ZipOutputStream(new FileOutputStream(zipFilePath))) {
            //递归的压缩文件夹和文件
            doCompress("", filePath, zos);
            //必须
            zos.finish();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private static void doCompress(String parentFilePath, String filePath, ZipOutputStream zos) {
        File sourceFile = new File(filePath);
        if (!sourceFile.exists()) {
            return;
        }
        String zipEntryName = parentFilePath + "/" + sourceFile.getName();
        if (parentFilePath.isEmpty()) {
            zipEntryName = sourceFile.getName();
        }
        if (sourceFile.isDirectory()) {
            File[] childFiles = sourceFile.listFiles();
            if (Objects.isNull(childFiles)) {
                return;
            }
            for (File childFile : childFiles) {
                doCompress(zipEntryName, childFile.getAbsolutePath(), zos);
            }
        } else {
            int len = -1;
            byte[] buf = new byte[1024];
            try (InputStream input = new BufferedInputStream(new FileInputStream(sourceFile))) {
                zos.putNextEntry(new ZipEntry(zipEntryName));
                while ((len = input.read(buf)) != -1) {
                    zos.write(buf, 0, len);
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    private String processViewIntent(Intent intent) {
        return intent.getDataString();
    }

    //-----------------------------------------------------------------------
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if (m_instance != null) {
            Log.d(TAG, "App is already running... this won't work");
            Intent intent = getIntent();
            if (intent != null) {
                Log.d(TAG, "There's an intent waiting...");
                String sharedData = processViewIntent(intent);
                if (sharedData != null) {
                    Log.d(TAG, "It's a view intent");
                    Intent viewIntent = new Intent(getApplicationContext(), MyActivity.class);
                    viewIntent.setAction(Intent.ACTION_VIEW);
                    viewIntent.putExtra("sharedData", sharedData);
                    startActivity(viewIntent);
                }
            }
            finish();
            return;
        }
        m_instance = this;
        Log.d(TAG, "Android activity created");

        //唤醒锁（手机上不推荐使用，其它插电安卓系统可考虑，比如广告机等）
        //acquireWakeLock();
        mySerivece = new PersistService();
        mSensorManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
        countSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_STEP_COUNTER);
        initStepSensor();

        registSreenStatusReceiver();

        //状态栏
        context = getApplicationContext();  // 获取程序句柄
        // 设置状态栏颜色,需要安卓版本大于5.0
        this.setStatusBarColor("#F3F3F3");  //灰
        // 设置状态栏全透明
        // this.setStatusBarFullTransparent();
        //状态栏文字自适应
        getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_LIGHT_STATUS_BAR);

        Application application = this.getApplication();
        application.registerActivityLifecycleCallbacks(this);

        // 获取系统语言
        MyService.isZh(this);

        // 服务
        Intent bindIntent = new Intent(MyActivity.this, MyService.class);
        if (Build.VERSION.SDK_INT >= 26) {
            startForegroundService(bindIntent);

        } else {
            bindService(bindIntent, mCon, Context.BIND_AUTO_CREATE);
            startService(new Intent(bindIntent));
        }

        //Test
        //MyService.notify(getApplicationContext(), "Hello!");

        // 定时闹钟
        alarmCount = 0;
        alarmManager = (AlarmManager) getSystemService(ALARM_SERVICE);
        intent = new Intent(MyActivity.this, ClockActivity.class);
        pi = PendingIntent.getActivity(MyActivity.this, 0, intent, 0);

        // HomeKey
        registerReceiver(mHomeKeyEvent, new IntentFilter(Intent.ACTION_CLOSE_SYSTEM_DIALOGS));
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
        System.out.println("onPause...");
        super.onPause();

    }

    @Override
    public void onStop() {
        System.out.println("onStop...");
        super.onStop();

        //目前暂不需要，已采用新方法
        //QtApplication.invokeDelegate();
    }

    @Override
    protected void onDestroy() {
        Log.i(TAG, "onDestroy...");
        releaseWakeLock();

        //让系统自行处理，否则退出时有可能出现崩溃
        //if(mHomeKeyEvent!=null)
        //unregisterReceiver(mHomeKeyEvent);

        //if(mScreenStatusReceiver!=null)
        //unregisterReceiver(mScreenStatusReceiver);

        android.os.Process.killProcess(android.os.Process.myPid());
        super.onDestroy();

        //目前暂不需要，已采用新方法
        //QtApplication.invokeDelegate();
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
        //转至后台
        System.out.println("MyActivity onActivityStopped...");

    }

    @Override
    public void onActivitySaveInstanceState(Activity activity, Bundle outState) {

    }

    @Override
    public void onActivityDestroyed(Activity activity) {

    }

    //---------------------------------------------------------------------------
    class PersistService extends Service implements SensorEventListener {
        public BroadcastReceiver mReceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                if (mSensorManager != null) {//取消监听后重写监听，以保持后台运行
                    mSensorManager.unregisterListener(PersistService.this);
                    mSensorManager
                            .registerListener(
                                    PersistService.this,
                                    mSensorManager
                                            .getDefaultSensor(Sensor.TYPE_STEP_COUNTER),
                                    SensorManager.SENSOR_DELAY_NORMAL);
                }
            }
        };

        @Override
        public void onAccuracyChanged(Sensor sensor, int accuracy) {
            Log.i(TAG, "PersistService.onAccuracyChanged().");
        }

        @Override
        public void onSensorChanged(SensorEvent sensorEvent) {
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

    }

    //--------------------------------------------------------------------------------
    // 备用参考
   /* private static final List<ZipEntry> entries = new ArrayList<ZipEntry>();

    public static void unZipDirectory(String zipFileDirectory, String outputDirectory) throws ZipException, IOException {
        File file = new File(zipFileDirectory);
        File[] files = file.listFiles();
        for (int i = 0; i < files.length; i++) {
            if (files[i].getName().endsWith(".zip")) {
                unzip(zipFileDirectory + File.separator + files[i].getName(),
                        outputDirectory);
            }
        }
    }

    public static void unzip(String string, String outputDirectory)
            throws ZipException, IOException {
        ZipFile zipFile = new ZipFile(string);
        Enumeration<ZipEntry> enu = (Enumeration<ZipEntry>) zipFile.entries();
        while (enu.hasMoreElements()) {
            ZipEntry entry = (ZipEntry) enu.nextElement();
            if (entry.isDirectory()) {
                String fileName = entry.getName().substring(0,
                        entry.getName().length() - 1);
                String directoryPath = outputDirectory + File.separator
                        + fileName;
                File directory = new File(directoryPath);
                directory.mkdir();
            }
            entries.add(entry);
        }
        unzip(zipFile, entries, outputDirectory);
    }

    private static void unzip(ZipFile zipFile, List<ZipEntry> entries2,
                              String outputDirectory) throws IOException {
        // TODO Auto-generated method stub
        Iterator<ZipEntry> it = entries.iterator();
        while (it.hasNext()) {
            ZipEntry zipEntry = (ZipEntry) it.next();
            // MultiThreadEntry mte = new MultiThreadEntry(zipFile, zipEntry,
            //         outputDirectory);

            // Thread thread = new Thread(mte);
            // thread.start();
            bis = new BufferedInputStream(zipFile.getInputStream(zipEntry));
            try {
                unzipFiles(zipEntry, outputDirectory);
                Log.i(TAG, zipEntry.getName());
                Log.i(TAG, outputDirectory);
            } catch (IOException e) {
                try {
                    bis.close();
                } catch (IOException e1) {
                    e1.printStackTrace();
                }
            } finally {
                try {
                    bis.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    public static final int BUFFER_SIZE = 4096;
    private static BufferedInputStream bis;

    public static void unzipFiles(ZipEntry zipEntry, String outputDirectory)
            throws IOException {

        byte[] data = new byte[BUFFER_SIZE];
        String entryName = zipEntry.getName();
        entryName = new String(entryName.getBytes("GBK"));
        FileOutputStream fos = new FileOutputStream(outputDirectory
                + File.separator + entryName);
        if (zipEntry.isDirectory()) {

        } else {
            BufferedOutputStream bos = new BufferedOutputStream(fos,
                    BUFFER_SIZE);
            int count = 0;
            while ((count = bis.read(data, 0, BUFFER_SIZE)) != -1) {
                bos.write(data, 0, count);
            }
            bos.flush();
            bos.close();
        }
    }

    //读取压缩文件中的内容名称
    public static List<String> readZipFile(String file) throws Exception {
        List<String> list = new ArrayList<String>();
        InputStream in = new BufferedInputStream(new FileInputStream(file));
        ZipInputStream zin = new ZipInputStream(in);
        ZipEntry ze;
        while ((ze = zin.getNextEntry()) != null) {
            if (ze.isDirectory()) {
            } else {
                String zeName = new String(ze.getName().getBytes("iso-8859-1"), "utf-8");
                list.add(zeName);
            }
        }
        zin.closeEntry();
        return list;
    }

    public static void readZipFile2(String file) throws Exception {
        ZipFile zf = new ZipFile(file);
        InputStream in = new BufferedInputStream(new FileInputStream(file));
        ZipInputStream zin = new ZipInputStream(in);
        ZipEntry ze;
        while ((ze = zin.getNextEntry()) != null) {
            if (ze.isDirectory()) {
            } else {
                String zeName = new String(ze.getName().getBytes("iso-8859-1"), "utf-8");
                System.out.println("获取文件名称：" + zeName);
                long size = ze.getSize();
                if (size > 0) {
                    BufferedReader br = new BufferedReader(
                            new InputStreamReader(zf.getInputStream(ze)));
                    String line;
                    while ((line = br.readLine()) != null) {
                        // System.out.println(line);
                    }
                    br.close();
                }
            }
        }
        zin.closeEntry();
    }*/

    //----------------------------------------------------------------------------------------------
    /*
This method can parse out the real local file path from a file URI.
*/
    public String getUriPath(String uripath) {
        String URL = uripath;
        String str = "None";
        try {
            //if (Build.VERSION.SDK_INT >= 26) {
            str = URLDecoder.decode(URL, "UTF-8");
            //}
        } catch (Exception e) {
            System.err.println("Error : URLDecoder.decode");
            e.printStackTrace();
        }

        Log.i(TAG, "UriString  " + uripath);
        Log.i(TAG, "RealPath  " + str);
        return str;

    }

    private String getUriRealPath(Context ctx, Uri uri) {
        String ret = "";

        if (isAboveKitKat()) {
            // Android sdk version number bigger than 19.
            ret = getUriRealPathAboveKitkat(ctx, uri);
        } else {
            // Android sdk version number smaller than 19.
            ret = getImageRealPath(getContentResolver(), uri, null);
        }

        return ret;
    }

    /*
    This method will parse out the real local file path from the file content URI.
    The method is only applied to the android SDK version number that is bigger than 19.
    */
    private String getUriRealPathAboveKitkat(Context ctx, Uri uri) {
        String ret = "";

        if (ctx != null && uri != null) {

            if (isContentUri(uri)) {
                if (isGooglePhotoDoc(uri.getAuthority())) {
                    ret = uri.getLastPathSegment();
                } else {
                    ret = getImageRealPath(getContentResolver(), uri, null);
                }
            } else if (isFileUri(uri)) {
                ret = uri.getPath();
            } else if (isDocumentUri(ctx, uri)) {

                // Get uri related document id.
                String documentId = DocumentsContract.getDocumentId(uri);

                // Get uri authority.
                String uriAuthority = uri.getAuthority();

                if (isMediaDoc(uriAuthority)) {
                    String idArr[] = documentId.split(":");
                    if (idArr.length == 2) {
                        // First item is document type.
                        String docType = idArr[0];

                        // Second item is document real id.
                        String realDocId = idArr[1];

                        // Get content uri by document type.
                        Uri mediaContentUri = MediaStore.Images.Media.EXTERNAL_CONTENT_URI;
                        if ("image".equals(docType)) {
                            mediaContentUri = MediaStore.Images.Media.EXTERNAL_CONTENT_URI;
                        } else if ("video".equals(docType)) {
                            mediaContentUri = MediaStore.Video.Media.EXTERNAL_CONTENT_URI;
                        } else if ("audio".equals(docType)) {
                            mediaContentUri = MediaStore.Audio.Media.EXTERNAL_CONTENT_URI;
                        }

                        // Get where clause with real document id.
                        String whereClause = MediaStore.Images.Media._ID + " = " + realDocId;

                        ret = getImageRealPath(getContentResolver(), mediaContentUri, whereClause);
                    }

                } else if (isDownloadDoc(uriAuthority)) {
                    // Build download URI.
                    Uri downloadUri = Uri.parse("content://downloads/public_downloads");

                    // Append download document id at URI end.
                    Uri downloadUriAppendId = ContentUris.withAppendedId(downloadUri, Long.valueOf(documentId));

                    ret = getImageRealPath(getContentResolver(), downloadUriAppendId, null);

                } else if (isExternalStoreDoc(uriAuthority)) {
                    String idArr[] = documentId.split(":");
                    if (idArr.length == 2) {
                        String type = idArr[0];
                        String realDocId = idArr[1];

                        if ("primary".equalsIgnoreCase(type)) {
                            ret = Environment.getExternalStorageDirectory() + "/" + realDocId;
                        }
                    }
                }
            }
        }

        return ret;
    }

    /* Check whether the current android os version is bigger than KitKat or not. */
    private boolean isAboveKitKat() {
        boolean ret = false;
        ret = Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT;
        return ret;
    }

    /* Check whether this uri represent a document or not. */
    private boolean isDocumentUri(Context ctx, Uri uri) {
        boolean ret = false;
        if (ctx != null && uri != null) {
            ret = DocumentsContract.isDocumentUri(ctx, uri);
        }
        return ret;
    }

    /* Check whether this URI is a content URI or not.
     *  content uri like content://media/external/images/media/1302716
     *  */
    private boolean isContentUri(Uri uri) {
        boolean ret = false;
        if (uri != null) {
            String uriSchema = uri.getScheme();
            if ("content".equalsIgnoreCase(uriSchema)) {
                ret = true;
            }
        }
        return ret;
    }

    /* Check whether this URI is a file URI or not.
     *  file URI like file:///storage/41B7-12F1/DCIM/Camera/IMG_20180211_095139.jpg
     * */
    private boolean isFileUri(Uri uri) {
        boolean ret = false;
        if (uri != null) {
            String uriSchema = uri.getScheme();
            if ("file".equalsIgnoreCase(uriSchema)) {
                ret = true;
            }
        }
        return ret;
    }


    /* Check whether this document is provided by ExternalStorageProvider. Return true means the file is saved in external storage. */
    private boolean isExternalStoreDoc(String uriAuthority) {
        boolean ret = false;

        if ("com.android.externalstorage.documents".equals(uriAuthority)) {
            ret = true;
        }

        return ret;
    }

    /* Check whether this document is provided by DownloadsProvider. return true means this file is a downloaded file. */
    private boolean isDownloadDoc(String uriAuthority) {
        boolean ret = false;

        if ("com.android.providers.downloads.documents".equals(uriAuthority)) {
            ret = true;
        }

        return ret;
    }

    /*
    Check if MediaProvider provides this document, if true means this image is created in the android media app.
    */
    private boolean isMediaDoc(String uriAuthority) {
        boolean ret = false;

        if ("com.android.providers.media.documents".equals(uriAuthority)) {
            ret = true;
        }

        return ret;
    }

    /*
    Check whether google photos provide this document, if true means this image is created in the google photos app.
    */
    private boolean isGooglePhotoDoc(String uriAuthority) {
        boolean ret = false;

        if ("com.google.android.apps.photos.content".equals(uriAuthority)) {
            ret = true;
        }

        return ret;
    }

    /* Return uri represented document file real local path.*/
    private String getImageRealPath(ContentResolver contentResolver, Uri uri, String whereClause) {
        String ret = "";

        // Query the URI with the condition.
        Cursor cursor = contentResolver.query(uri, null, whereClause, null, null);

        if (cursor != null) {
            boolean moveToFirst = cursor.moveToFirst();
            if (moveToFirst) {

                // Get columns name by URI type.
                String columnName = MediaStore.Images.Media.DATA;

                if (uri == MediaStore.Images.Media.EXTERNAL_CONTENT_URI) {
                    columnName = MediaStore.Images.Media.DATA;
                } else if (uri == MediaStore.Audio.Media.EXTERNAL_CONTENT_URI) {
                    columnName = MediaStore.Audio.Media.DATA;
                } else if (uri == MediaStore.Video.Media.EXTERNAL_CONTENT_URI) {
                    columnName = MediaStore.Video.Media.DATA;
                }

                // Get column index.
                int imageColumnIndex = cursor.getColumnIndex(columnName);

                // Get column value which is the uri related file local path.
                ret = cursor.getString(imageColumnIndex);
            }
        }

        return ret;
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
                    System.out.println("MyActivity HOME键被按下...");
                } else if (TextUtils.equals(reason, SYSTEM_HOME_KEY_LONG)) {
                    // 表示长按home键,显示最近使用的程序
                    System.out.println("MyActivity 长按HOME键...");
                }
            }
        }
    };

    //---------------------------------------------------------------------------------------------
    static public int copyFile(String srcPath, String FileName) {

        Log.i(TAG, "src  " + srcPath);
        Log.i(TAG, "dest  " + FileName);

        int result = 0;
        if ((srcPath == null) || (FileName == null)) {
            return result;
        }
        File src = new File(srcPath);
        //File dest = new File("/storage/emulated/0/Download/ " + FileName);
        File dest = new File(FileName);
        if (dest != null && dest.exists()) {
            dest.delete(); // delete file
        }
        try {
            dest.createNewFile();
        } catch (IOException e) {
            e.printStackTrace();
        }

        FileChannel srcChannel = null;
        FileChannel dstChannel = null;

        try {
            srcChannel = new FileInputStream(src).getChannel();
            dstChannel = new FileOutputStream(dest).getChannel();
            srcChannel.transferTo(0, srcChannel.size(), dstChannel);
            result = 1;
        } catch (FileNotFoundException e) {
            e.printStackTrace();
            return result;
        } catch (IOException e) {
            e.printStackTrace();
            return result;
        }
        try {
            srcChannel.close();
            dstChannel.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return result;
    }

    //----------------------------------------------------------------------------------------------
    private final static int REQUEST_CODE = 100;

    public void openKnotBakDir() {
        Uri dir = Uri.parse("/storage/emulated/0/KnotBak/");
        Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT);
        intent.putExtra(DocumentsContract.EXTRA_INITIAL_URI, dir);
        intent.setType("*/*");
        intent.addCategory(Intent.CATEGORY_OPENABLE);
        startActivityForResult(intent, REQUEST_CODE);
    }

    //----------------------------------------------------------------------------------------------
    public static int getAndroidVer() {
        int a = Build.VERSION.SDK_INT;

        Log.i(TAG, "os ver=" + a);
        return a;
    }

    private static String souAPK;

    public static void setAPKFile(String apkfile) {
        souAPK = apkfile;
    }

    public void installApk() {
        Log.i(TAG, "install APK file=" + souAPK + "  PackgeName=" + context.getPackageName());
        File apkFile = new File(souAPK);

        //判断是否是AndroidN以及更高的版本
        //if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N)
        if (Build.VERSION.SDK_INT >= 24)
        {
            Intent intent = new Intent(Intent.ACTION_VIEW);
            intent.setFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
            Uri contentUri = FileProvider.getUriForFile(context, context.getPackageName(), apkFile);

            intent.setDataAndType(contentUri, "application/vnd.android.package-archive");
            Log.i(TAG, "contentUri=" + contentUri);
            startActivity(intent);
        }
        if (Build.VERSION.SDK_INT < 24) {
            //intent.setDataAndType(Uri.fromFile(apkFile), "application/vnd.android.package-archive");
            //另外一种方法
            //intent.setDataAndType(Uri.parse("file://" + filePath), "application/vnd.android.package-archive");
            //intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);

            Uri uri = Uri.fromFile(apkFile);
            Intent intent = new Intent();
            intent.setClassName("com.android.packageinstaller", "com.android.packageinstaller.PackageInstallerActivity");
            intent.setData(uri);
            intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            startActivity(intent);
        }

    }
}


