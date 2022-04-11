
package com;

import org.qtproject.qt5.android.bindings.QtApplication;
import org.qtproject.qt5.android.bindings.QtActivity;//add QtActivity
import android.app.ActivityManager;
import android.app.ActivityManager.RunningAppProcessInfo;

import android.content.Intent;
import android.content.pm.ResolveInfo;
import android.content.pm.PackageManager;

public class mmJavaActivity  extends org.qtproject.qt5.android.bindings.QtActivity //add  extends QtActivity.
{
     private static mmJavaActivity m_instance ;

     public  mmJavaActivity()
     {
         m_instance = this;
     }

    public static  int  mini()
     {
         System.out.println("+++++++++++++++++++++++");

         m_instance.moveTaskToBack(true);

         return 1;
     }


}
