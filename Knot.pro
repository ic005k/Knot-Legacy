QT += core gui network
QT += charts sensors
QT += qml quick quickwidgets webview
QT += xml

# Qt > 5 (Qt6)
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets #webenginewidgets

CONFIG += c++11
CONFIG+=sdk_no_version_check

ICON = res/icon.icns
TRANSLATIONS += src/cn.ts

unix:!macx:{

}

android: {
    lessThan(QT_MAJOR_VERSION, 6):QT += androidextras
}


android
{
ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
#dataFiles.files+=src/readme.txt
#dataFiles.files+=src/1.png
#dataFiles.files+=src/2.png
dataFiles.path = /assets/data
INSTALLS += dataFiles
}

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/Preferences.cpp \
    src/RemarksAbout.cpp \
    src/Report.cpp \
    src/autoupdatedialog.cpp \
    src/dlgMainNotes.cpp \
    src/dlgNotesList.cpp \
    src/dlgOneDrive.cpp \
    src/dlgReaderFun.cpp \
    src/dlgSetText.cpp \
    src/dlgfloatfun.cpp \
    src/dlgleft.cpp \
    src/dlglist.cpp \
    src/dlgloadpic.cpp \
    src/dlgreader.cpp \
    src/dlgrename.cpp \
    src/dlgright.cpp \
    src/dlgsetedittext.cpp \
    src/dlgsettime.cpp \
    src/dlgsteps.cpp \
    src/dlgtodo.cpp \
    src/documenthandler.cpp \
    src/file.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/onedrive/dlgweb.cpp \
    src/onedrive/qtonedrive.cpp \
    src/onedrive/qtonedriveauthorizationdialog.cpp \
    src/setTodoAlarm.cpp \
    src/specialaccelerometerpedometer.cpp

HEADERS += \
    src/Preferences.h \
    src/RemarksAbout.h \
    src/Report.h \
    src/autoupdatedialog.h \
    src/count_steps.h \
    src/dlgMainNotes.h \
    src/dlgNotesList.h \
    src/dlgOneDrive.h \
    src/dlgReaderFun.h \
    src/dlgSetText.h \
    src/dlgfloatfun.h \
    src/dlgleft.h \
    src/dlglist.h \
    src/dlgloadpic.h \
    src/dlgreader.h \
    src/dlgrename.h \
    src/dlgright.h \
    src/dlgsetedittext.h \
    src/dlgsteps.h \
    src/dlgsettime.h \
    src/dlgtodo.h \
    src/documenthandler.h \
    src/file.h \
    src/mainwindow.h \
    src/onedrive/dlgweb.h \
    src/onedrive/qtonedrive.h \
    src/onedrive/qtonedriveauthorizationdialog.h \
    src/onedrive/qtonedrivelib_global.h \
    src/setTodoAlarm.h \
    src/specialaccelerometerpedometer.h

FORMS += \
    src/Preferences.ui \
    src/RemarksAbout.ui \
    src/Report.ui \
    src/autoupdatedialog.ui \
    src/dlgMainNotes.ui \
    src/dlgNotesList.ui \
    src/dlgOneDrive.ui \
    src/dlgReaderFun.ui \
    src/dlgSetText.ui \
    src/dlgfloatfun.ui \
    src/dlgleft.ui \
    src/dlglist.ui \
    src/dlgloadpic.ui \
    src/dlgreader.ui \
    src/dlgrename.ui \
    src/dlgright.ui \
    src/dlgsetedittext.ui \
    src/dlgsettime.ui \
    src/dlgsteps.ui \
    src/dlgtodo.ui \
    src/mainwindow.ui \
    src/onedrive/dlgweb.ui \
    src/setTodoAlarm.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    qrc.qrc \
    res.qrc

CONFIG(debug,debug|release) {
    #DESTDIR = $$absolute_path($${_PRO_FILE_PWD_}/bin/debug)
} else {
    #DESTDIR = $$absolute_path($${_PRO_FILE_PWD_}/bin/release)
}

DISTFILES += \
    android/AndroidManifest.xml \
    android/build.gradle \
    android/build.gradle \
    android/gradle.properties \
    android/gradle.properties \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew \
    android/gradlew.bat \
    android/gradlew.bat \
    android/res/values/libs.xml \
    android/res/values/libs.xml \
    src/cn.qm \
    src/cn.ts \
    src/imgview.qml \
    src/memo.qml \
    src/onedrive/log.qml \
    src/onedrive/web.qml \
    src/reader.qml \
    src/steps.qml

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
android: include(C:/Users/Administrator/AppData/Local/Android/Sdk/android_openssl/openssl.pri)
android: include(/Users/hz/Library/Android/sdk/android_openssl/openssl.pri)
