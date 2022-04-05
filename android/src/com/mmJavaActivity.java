
package com;

import org.qtproject.qt5.android.bindings.QtActivity;//add QtActivity
import android.app.ActivityManager;
import android.app.ActivityManager.RunningAppProcessInfo;

public class mmJavaActivity  extends org.qtproject.qt5.android.bindings.QtActivity //add  extends QtActivity.
{
     private static mmJavaActivity m_instance ;

     public mmJavaActivity()
     {
         m_instance = this;
     }

    public  static int  mini()
     {
         m_instance.moveTaskToBack(true);

         return 1;
     }



}
