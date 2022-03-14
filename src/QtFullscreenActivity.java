package an.qt.QtFullscreenActivityAPP;//指明文件包名
import android.content.Context;
import android.content.Intent;
import android.app.PendingIntent;
import android.util.Log;
import android.os.Bundle;
import android.view.WindowManager;

//继承 QtActivity 类
public class QtFullscreenActivity extends org.qtproject.qt5.android.bindings.QtActivity
{
    private final static String TAG = "QtFullscreen";
    @Override
	// 重写 onCreate 方法
    public void onCreate(Bundle savedInstanceState) {
          super.onCreate(savedInstanceState);
		  // 设置全屏并且状态栏悬浮程序之上
          getWindow().addFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS);
          getWindow().getDecorView().setFitsSystemWindows(true);
    }
}
