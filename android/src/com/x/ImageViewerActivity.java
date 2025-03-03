package com.x;

import com.x.MDActivity;
import com.x.MyActivity;
import com.x.ZoomableImageView;

import android.view.LayoutInflater;
import android.view.Gravity;
import android.content.IntentFilter;
import android.content.Intent;
import android.content.BroadcastReceiver;
import android.app.PendingIntent;
import android.text.TextUtils;
import java.lang.CharSequence;
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
import java.io.FileReader;
import java.io.FileInputStream;

import java.util.HashMap;
import java.util.Locale;
import java.util.Map;
import java.util.Properties;
import java.io.IOException;
import java.io.File;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
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
import java.net.URISyntaxException;
import java.sql.Time;
import android.text.SpannableStringBuilder;
import android.text.style.BackgroundColorSpan;
import android.text.style.ForegroundColorSpan;
import android.text.Spannable;
import android.text.Spanned;
import android.view.MenuItem;
import android.net.Uri;
import android.text.method.ScrollingMovementMethod;
import android.view.inputmethod.EditorInfo;
import android.view.KeyEvent;
import android.widget.TextView.OnEditorActionListener;
import android.view.inputmethod.InputMethodManager;
import java.util.regex.*;

import android.widget.LinearLayout;
import android.view.LayoutInflater;
import android.widget.PopupWindow;
import android.graphics.drawable.ColorDrawable;
import java.lang.reflect.Field;
import android.annotation.SuppressLint;
import androidx.core.content.FileProvider;
import android.widget.PopupMenu;
import android.widget.ImageButton;
import android.content.SharedPreferences;
import android.widget.ScrollView;
import android.text.method.LinkMovementMethod;

import android.text.method.MovementMethod;
import android.text.method.Touch;
import android.view.MotionEvent;
import android.view.ViewTreeObserver;
import android.widget.ImageView;
import android.util.DisplayMetrics;
import android.view.ViewGroup;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import android.text.util.Linkify;

import java.util.Iterator;

import java.io.BufferedReader;
import java.io.InputStreamReader;

import java.io.IOException;
import java.io.InputStream;

import androidx.appcompat.view.ContextThemeWrapper;
import androidx.core.content.ContextCompat;
import android.graphics.Typeface;

import android.os.Bundle;
import android.widget.TextView;
import androidx.appcompat.app.AppCompatActivity;
import androidx.annotation.NonNull;
import android.text.style.ClickableSpan;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.URL;

public class ImageViewerActivity extends Activity {

    private static final String TAG = "ImageViewerActivity";
    private ZoomableImageView imageView;
    private TextView filePathTextView;
    private Button shareButton;

    private String imagePath;

    private Handler handler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            if (msg.what == 1) {
                Bitmap bitmap = (Bitmap) msg.obj;
                imageView.setImageBitmap(bitmap);
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_image_viewer);

        // 去除title(App Name)
        requestWindowFeature(Window.FEATURE_NO_TITLE);

        imageView = findViewById(R.id.image_view);
        filePathTextView = findViewById(R.id.file_path_text_view);
        shareButton = findViewById(R.id.share_button);

        imagePath = MyActivity.strImageFile;
        if (imagePath != null) {
            filePathTextView.setText(imagePath);
            loadImage(imagePath);
        }

        shareButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                shareImage(MyActivity.strImageFile);
            }
        });
    }

    private void loadImage(String imagePath) {
        if (imagePath.startsWith("http")) {
            new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        URL url = new URL(imagePath);
                        HttpURLConnection connection = (HttpURLConnection) url.openConnection();
                        connection.setDoInput(true);
                        connection.connect();
                        InputStream input = connection.getInputStream();
                        Bitmap bitmap = BitmapFactory.decodeStream(input);
                        Message message = new Message();
                        message.what = 1;
                        message.obj = bitmap;
                        handler.sendMessage(message);
                        input.close();
                    } catch (IOException e) {
                        Log.e(TAG, "Error loading image: " + e.getMessage());
                    }
                }
            }).start();
        } else {
            Bitmap bitmap = BitmapFactory.decodeFile(imagePath);
            imageView.setImageBitmap(bitmap);
        }
    }

    private void shareImage(String path) {
        Intent share = new Intent(Intent.ACTION_SEND);
        share.setType("image/*"); // "image/png"

        Uri photoUri;
        if (Build.VERSION.SDK_INT >= 24) {
            photoUri = FileProvider.getUriForFile(
                    MyActivity.context,
                    MyActivity.context.getPackageName(),
                    new File(path));
        } else {
            photoUri = Uri.fromFile(new File(path));
        }
        System.out.println("path=" + path + "  pathUri=" + photoUri);
        share.putExtra(Intent.EXTRA_STREAM, photoUri);
        MyActivity.context.startActivity(Intent.createChooser(share, "Note Image"));
    }
}
