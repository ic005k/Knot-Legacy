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
import android.net.Uri;
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
import android.text.TextWatcher;
import android.os.Handler;
import android.media.AudioManager;
import android.widget.TextView;
import android.content.DialogInterface;
import java.util.Locale;
import android.app.Application;
import android.os.Looper;
import java.io.File;
import java.io.IOException;
import java.io.RandomAccessFile;
import android.provider.MediaStore;
import android.provider.DocumentsContract;
import android.database.Cursor;
import android.content.ContentUris;
import android.os.Environment;
import android.os.Message;
import android.view.Menu;
import android.widget.Toast;
import java.lang.reflect.Method;
import java.net.URI;
import java.sql.Time;

import android.view.MenuItem;
import android.widget.PopupMenu;

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
    private Button btnMenu;
    public static EditText editNote;
    private static boolean zh_cn;
    private String currentMDFile;
    private static Context context;
    private static NoteEditor m_instance;
    private static boolean isTextChanged = false;

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
        btnMenu = (Button) findViewById(R.id.btnMenu);

        if (zh_cn) {
            btn_cancel.setText("关闭");
            btnInsertImg.setText("插入图片");
            btnMenu.setText("快捷输入");
        } else {
            btn_cancel.setText("Close");
            btnInsertImg.setText("Insert Img");
            btnMenu.setText("Quick Input");
        }
        btn_cancel.setOnClickListener(this);
        btnInsertImg.setOnClickListener(this);
        btnMenu.setOnClickListener(this);

    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btn_cancel:
                onBackPressed();
                break;

            case R.id.btnInsertImg:
                openFilePicker();
                break;

            case R.id.btnMenu:
                showPopupMenu(btnMenu);
                break;
        }
    }

    private void AnimationWhenClosed() {
        // 淡出效果
        // overridePendingTransition(android.R.anim.fade_in, android.R.anim.fade_out);

        // 或者使用底部滑出效果(自定义文件exit_anim.xml)
        overridePendingTransition(0, R.anim.exit_anim);
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        context = getApplicationContext();
        // this.getWindow().setWindowAnimations(R.style.WindowAnim);
        isZh(context);
        m_instance = this;
        registerReceiver(mHomeKeyEvent, new IntentFilter(Intent.ACTION_CLOSE_SYSTEM_DIALOGS));
        Application application = this.getApplication();
        application.registerActivityLifecycleCallbacks(this);

        String filename = "/storage/emulated/0/.Knot/mymd.txt";
        strInfo = readFile(filename);

        // 去除title(App Name)
        requestWindowFeature(Window.FEATURE_NO_TITLE);

        // 去掉Activity上面的状态栏(Show Time...)
        // getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
        // WindowManager.LayoutParams.FLAG_FULLSCREEN);

        this.setStatusBarColor("#F3F3F3"); // 灰
        getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_LIGHT_STATUS_BAR);

        setContentView(R.layout.noteeditor);
        bindViews(strInfo);

        // set cursor pos
        filename = "/storage/emulated/0/.Knot/note_text.ini";
        String s_cpos = null;

        /*
         * internalConfigure = new InternalConfigure(this);
         * try {
         * internalConfigure.readFrom(filename);
         * } catch (Exception e) {
         * System.err.println("Error : reading note_text.ini");
         * e.printStackTrace();
         * }
         * s_cpos = internalConfigure.getIniKey(currentMDFile);
         * currentMDFile = internalConfigure.getIniKey("currentMDFile");
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

        isTextChanged = false;
        initEditNote();

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

        if (isTextChanged) {
            // showNormalDialog();
            saveNote();

        }

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
            // 设置Context.MODE_PRIVATE表示每次调用该方法会覆盖原来的文件数据
            FileOutputStream fileOutputStream;// = context.openFileOutput(filename, Context.MODE_PRIVATE);

            // 每次都会生成一个新文件并抹掉除本次key之外的其他key数据
            // File file = new File(filename);
            fileOutputStream = new FileOutputStream(filename);

            // 通过properties.stringPropertyNames()获得所有key的集合Set，里面是String对象
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

    // fileName 为文件名称 返回true为存在
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
        // while ((line = br.readLine()) != null) {
        // System.out.println(line);
        // }

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
            // 打开文件输入流
            FileInputStream inputStream = new FileInputStream(file);
            byte[] buffer = new byte[1024];
            int len = inputStream.read(buffer);
            // 读取文件内容
            while (len > 0) {
                sb.append(new String(buffer, 0, len));
                // 继续将数据放到buffer中
                len = inputStream.read(buffer);
            }
            // 关闭输入流
            inputStream.close();
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        return sb.toString();
    }

    // 将字符串写入到文本文件中
    public void writeTxtToFile(String content, String filePath, String fileName) {
        // 生成文件夹之后，再生成文件，不然会出错
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

    // 生成文件
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

    // 生成文件夹
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

    public static void appendNote(String str) {
        editNote.append("\n\n");
        editNote.append(str);

    }

    public static void insertNote(String str) {

        int index = editNote.getSelectionStart();// 获取光标所在位置
        Editable edit = editNote.getEditableText();// 获取EditText的文字
        if (index < 0 || index >= edit.length()) {
            edit.append(str);
        } else {
            edit.insert(index, str);// 光标所在位置插入文字
        }

    }

    private void saveNote() {
        // save current text
        String mContent = editNote.getText().toString();
        String mPath = "/storage/emulated/0/.Knot/";
        writeTxtToFile(mContent, mPath, "note_text.txt");

        // save cursor pos
        String file2 = "/storage/emulated/0/.Knot/note_text.ini";
        int cpos = editNote.getSelectionStart();

        /*
         * internalConfigure = new InternalConfigure(this);
         * Properties myPro = new Properties();
         * myPro.put(currentMDFile, String.valueOf(cpos));
         * try {
         * internalConfigure.saveFile(file2, myPro);
         * } catch (Exception e) {
         * System.err.println("Error : save note_text.ini");
         * e.printStackTrace();
         * }
         */

        try {
            Wini ini = new Wini(new File(file2));

            ini.put("cpos", currentMDFile, String.valueOf(cpos));

            ini.store();
        } catch (IOException e) {
            e.printStackTrace();
        }

        CallJavaNotify_6();
    }

    private void openFilePicker() {
        Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT);
        intent.setType("image/*"); // "text/plain" 只显示 txt 文件
        startActivityForResult(intent, 1);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (resultCode == RESULT_OK && requestCode == 1) {
            Uri selectedFileUri = data.getData();
            String filePath = "/storage/emulated/0/.Knot/receive_share_pic.png";
            readFileFromUriToLocal(selectedFileUri, filePath);

            // if (Build.VERSION.SDK_INT >= 24)
            // filePath = selectedFileUri.getPath();
            // else // Android 6.0及以下
            // filePath = getPath(this, selectedFileUri);

            // 处理文件路径
            handleFilePath(filePath);
        }
    }

    private void handleFilePath(String filePath) {
        // 处理文件路径的逻辑

        System.out.println("open image file=" + filePath);
        // MyActivity.copyFile(filePath,

        // "/storage/emulated/0/.Knot/receive_share_pic.png");
        if (fileIsExists(filePath))
            CallJavaNotify_7();
        else
            Toast.makeText(this, filePath + "  The file does not exist.", 12000).show();
    }

    /**
     * Get a file path from a Uri. This will get the the path for Storage Access
     * Framework Documents, as well as the _data field for the MediaStore and
     * other file-based ContentProviders.
     *
     * @param context The context.
     * @param uri     The Uri to query.
     * @author paulburke
     */
    public static String getPath(final Context context, final Uri uri) {

        final boolean isKitKat = Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT;
        ;

        // DocumentProvider
        if (isKitKat && DocumentsContract.isDocumentUri(context, uri)) {
            // ExternalStorageProvider
            if (isExternalStorageDocument(uri)) {
                final String docId = DocumentsContract.getDocumentId(uri);
                final String[] split = docId.split(":");
                final String type = split[0];

                if ("primary".equalsIgnoreCase(type)) {
                    return Environment.getExternalStorageDirectory() + "/" + split[1];
                }

                // TODO handle non-primary volumes
            }
            // DownloadsProvider
            else if (isDownloadsDocument(uri)) {

                final String id = DocumentsContract.getDocumentId(uri);
                final Uri contentUri = ContentUris.withAppendedId(
                        Uri.parse("content://downloads/public_downloads"), Long.valueOf(id));

                return getDataColumn(context, contentUri, null, null);
            }
            // MediaProvider
            else if (isMediaDocument(uri)) {
                final String docId = DocumentsContract.getDocumentId(uri);
                final String[] split = docId.split(":");
                final String type = split[0];

                Uri contentUri = null;
                if ("image".equals(type)) {
                    contentUri = MediaStore.Images.Media.EXTERNAL_CONTENT_URI;
                } else if ("video".equals(type)) {
                    contentUri = MediaStore.Video.Media.EXTERNAL_CONTENT_URI;
                } else if ("audio".equals(type)) {
                    contentUri = MediaStore.Audio.Media.EXTERNAL_CONTENT_URI;
                }

                final String selection = "_id=?";
                final String[] selectionArgs = new String[] {
                        split[1]
                };

                return getDataColumn(context, contentUri, selection, selectionArgs);
            }
        }
        // MediaStore (and general)
        else if ("content".equalsIgnoreCase(uri.getScheme())) {

            // Return the remote address
            if (isGooglePhotosUri(uri))
                return uri.getLastPathSegment();

            return getDataColumn(context, uri, null, null);
        }
        // File
        else if ("file".equalsIgnoreCase(uri.getScheme())) {
            return uri.getPath();
        }

        return null;
    }

    /**
     **
     * Get the value of the data column for this Uri. This is useful for
     * MediaStore Uris, and other file-based ContentProviders.
     *
     * @param context       The context.
     * @param uri           The Uri to query.
     * @param selection     (Optional) Filter used in the query.
     * @param selectionArgs (Optional) Selection arguments used in the query.
     * @return The value of the _data column, which is typically a file path.
     */
    public static String getDataColumn(Context context, Uri uri, String selection,
            String[] selectionArgs) {

        Cursor cursor = null;
        final String column = "_data";
        final String[] projection = {
                column
        };

        try {
            cursor = context.getContentResolver().query(uri, projection, selection, selectionArgs,
                    null);
            if (cursor != null && cursor.moveToFirst()) {
                final int index = cursor.getColumnIndexOrThrow(column);
                return cursor.getString(index);
            }
        } finally {
            if (cursor != null)
                cursor.close();
        }
        return null;
    }

    /**
     * @param uri The Uri to check.
     * @return Whether the Uri authority is ExternalStorageProvider.
     */
    public static boolean isExternalStorageDocument(Uri uri) {
        return "com.android.externalstorage.documents".equals(uri.getAuthority());
    }

    /**
     * @param uri The Uri to check.
     * @return Whether the Uri authority is DownloadsProvider.
     */
    public static boolean isDownloadsDocument(Uri uri) {
        return "com.android.providers.downloads.documents".equals(uri.getAuthority());
    }

    /**
     * @param uri The Uri to check.
     * @return Whether the Uri authority is MediaProvider.
     */
    public static boolean isMediaDocument(Uri uri) {
        return "com.android.providers.media.documents".equals(uri.getAuthority());
    }

    /**
     * @param uri The Uri to check.
     * @return Whether the Uri authority is Google Photos.
     */
    public static boolean isGooglePhotosUri(Uri uri) {
        return "com.google.android.apps.photos.content".equals(uri.getAuthority());
    }

    private void initEditNote() {

        editNote.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {

            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {

            }

            @Override
            public void afterTextChanged(Editable editable) {
                isTextChanged = true;
            }
        });
    }

    private void showNormalDialog() {
        /*
         * @setIcon 设置对话框图标
         * 
         * @setTitle 设置对话框标题
         * 
         * @setMessage 设置对话框消息提示
         * setXXX方法返回Dialog对象，因此可以链式设置属性
         */
        final AlertDialog.Builder normalDialog = new AlertDialog.Builder(NoteEditor.this);
        // normalDialog.setIcon(R.drawable.alarm.png);
        normalDialog.setTitle("Knot");
        normalDialog.setMessage("The text has been modified. Do you want to save?");
        normalDialog.setPositiveButton("Yes",
                new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        // ...To-do
                        saveNote();

                    }
                });
        normalDialog.setNegativeButton("No",
                new DialogInterface.OnClickListener() {

                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        // ...To-do
                    }
                });
        // 显示
        normalDialog.show();

    }

    /* 创建选项菜单，目前暂不使用，它显示在菜单栏上 */
    // 该方法用于创建显示Menu

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_optionmenu, menu);
        return true;
    }

    // 在选项菜单打开以后会调用这个方法，设置menu图标显示（icon）

    @Override
    public boolean onMenuOpened(int featureId, Menu menu) {
        if (menu != null) {
            if (menu.getClass().getSimpleName().equalsIgnoreCase("MenuBuilder")) {
                try {
                    Method method = menu.getClass().getDeclaredMethod("setOptionalIconsVisible",
                            Boolean.TYPE);
                    method.setAccessible(true);
                    method.invoke(menu, true);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }
        return super.onMenuOpened(featureId, menu);
    }

    // 该方法对菜单的item进行监听

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // mTextView.setText(item.getTitle());
        switch (item.getItemId()) {
            case R.id.menu1:
                Toast.makeText(this, "点击了第" + 1 + "个", Toast.LENGTH_SHORT).show();
                break;
            case R.id.menu2:
                Toast.makeText(this, "点击了第" + 2 + "个", Toast.LENGTH_SHORT).show();
                break;
            case R.id.menu3:
                Toast.makeText(this, "点击了第" + 3 + "个", Toast.LENGTH_SHORT).show();
                break;
            case R.id.menu4:
                Toast.makeText(this, "点击了第" + 4 + "个", Toast.LENGTH_SHORT).show();
                break;
            case R.id.menu5:
                Toast.makeText(this, "点击了第" + 5 + "个", Toast.LENGTH_SHORT).show();
                break;
        }
        return super.onOptionsItemSelected(item);
    }

    private void showPopupMenu(View view) {
        // View当前PopupMenu显示的相对View的位置
        PopupMenu popupMenu = new PopupMenu(this, view);
        // menu布局
        popupMenu.getMenuInflater().inflate(R.menu.main, popupMenu.getMenu());
        // menu的item点击事件
        popupMenu.setOnMenuItemClickListener(new PopupMenu.OnMenuItemClickListener() {
            @Override
            public boolean onMenuItemClick(MenuItem item) {

                // test
                switch (item.getItemId()) {
                    case R.id.s1:

                        break;

                }

                if (item.getTitle().equals("Table")) {

                    String str1 = "|Title1|Title2|Title3|\n";
                    String str2 = "|------|------|----- |\n";
                    String str3 = "|        |        |        |\n";
                    String str4 = "|        |        |        |\n";

                    insertNote(str1 + str2 + str3 + str4);

                }

                if (item.getTitle().equals("Bold")) {
                    String sel = getEditSelectText();
                    if (sel.length() > 0) {
                        delEditSelectText();
                        insertNote("**" + sel + "**");
                    } else
                        insertNote("**Bold**");
                }

                if (item.getTitle().equals("Italic")) {
                    String sel = getEditSelectText();
                    if (sel.length() > 0) {
                        delEditSelectText();
                        insertNote("_" + sel + "_");
                    } else
                        insertNote("_Italic_");
                }

                if (item.getTitle().equals("Font Color")) {
                    String sel = getEditSelectText();
                    if (sel.length() > 0) {
                        delEditSelectText();
                        insertNote("<font color=#E01B24>" + sel + "</font>");
                    } else
                        insertNote("<font color=#E01B24>Color</font>");
                }

                if (item.getTitle().equals("#")) {
                    insertNote("#");
                }

                if (item.getTitle().equals(">"))
                    insertNote("> ");

                if (item.getTitle().equals("Date")) {
                    SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd");
                    String dateString = sdf.format(new Date());
                    insertNote(dateString);
                }

                if (item.getTitle().equals("Time")) {
                    SimpleDateFormat sdf = new SimpleDateFormat("HH:mm:ss");
                    String timeString = sdf.format(new Date());
                    insertNote(timeString);
                }

                if (item.getTitle().equals("Link")) {
                    insertNote("[]()");
                }

                // Toast.makeText(getApplicationContext(), item.getTitle(),
                // Toast.LENGTH_SHORT).show();
                return false;
            }
        });
        // PopupMenu关闭事件
        popupMenu.setOnDismissListener(new PopupMenu.OnDismissListener() {
            @Override
            public void onDismiss(PopupMenu menu) {
                // Toast.makeText(getApplicationContext(), "关闭PopupMenu",
                // Toast.LENGTH_SHORT).show();
            }
        });

        popupMenu.show();
    }

    public boolean readFileFromUriToLocal(Uri uri, String localfile) {

        try {
            // 之前的目录存储位置备注：getExternalFilesDir(null).getPath() + File.separator =
            // /storage/emulated/0/Android/data/com.x/files
            File outFile = new File(localfile);
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

        return false;
    }

    public String getEditSelectText() {
        Editable editable = editNote.getText();
        int beg = editNote.getSelectionStart();
        int end = editNote.getSelectionEnd();
        String sel = editable.toString().substring(beg, end);

        return sel;

    }

    public void delEditSelectText() {
        int beg = editNote.getSelectionStart();
        int end = editNote.getSelectionEnd();
        Editable editable = editNote.getText();
        editable.delete(beg, end);
    }

}
