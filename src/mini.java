
package com.android.activity;

import org.qtproject.qt5.android.bindings.QtActivity;//add QtActivity
import android.app.ActivityManager;
import android.app.ActivityManager.RunningAppProcessInfo;


public class JavaActivity  extends org.qtproject.qt5.android.bindings.QtActivity //add  extends QtActivity.
{
     private static JavaActivity m_instance ;

     public JavaActivity()
     {
         m_instance = this;
     }
    public  static int  mini()
    {
        m_instance.moveTaskToBack(true);
        System.out.println("+++++++++++++++++++++++");
        return 1;
    }
}
