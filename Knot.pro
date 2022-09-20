QT += core gui network
QT += charts sensors
QT += qml quick quickwidgets webview
QT += printsupport
QT += xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets #webenginewidgets

CONFIG += c++11

ICON = res/icon.icns
TRANSLATIONS += src/cn.ts

unix:!macx:{
    QT += androidextras
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
    src/chart.cpp \
    src/dlgMemos.cpp \
    src/dlgOneDrive.cpp \
    src/dlgfloatfun.cpp \
    src/dlglist.cpp \
    src/dlgloadpic.cpp \
    src/dlgnotes.cpp \
    src/dlgpreferences.cpp \
    src/dlgreader.cpp \
    src/dlgrename.cpp \
    src/dlgreport.cpp \
    src/dlgsettime.cpp \
    src/dlgsteps.cpp \
    src/dlgtodo.cpp \
    src/documenthandler.cpp \
    src/file.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/msgdialog.cpp \
    src/onedrive/dlgweb.cpp \
    src/onedrive/qtonedrive.cpp \
    src/onedrive/qtonedriveauthorizationdialog.cpp \
    src/smoothscrollbar.cpp \
    src/specialaccelerometerpedometer.cpp

HEADERS += \
    src/chart.h \
    src/count_steps.h \
    src/dlgMemos.h \
    src/dlgOneDrive.h \
    src/dlgfloatfun.h \
    src/dlglist.h \
    src/dlgloadpic.h \
    src/dlgnotes.h \
    src/dlgpreferences.h \
    src/dlgreader.h \
    src/dlgrename.h \
    src/dlgreport.h \
    src/dlgsteps.h \
    src/dlgsettime.h \
    src/dlgtodo.h \
    src/documenthandler.h \
    src/file.h \
    src/mainwindow.h \
    src/msgdialog.h \
    src/onedrive/dlgweb.h \
    src/onedrive/qtonedrive.h \
    src/onedrive/qtonedriveauthorizationdialog.h \
    src/onedrive/qtonedrivelib_global.h \
    src/smoothscrollbar.h \
    src/specialaccelerometerpedometer.h

FORMS += \
    src/dlgMemos.ui \
    src/dlgOneDrive.ui \
    src/dlgfloatfun.ui \
    src/dlglist.ui \
    src/dlgloadpic.ui \
    src/dlgnotes.ui \
    src/dlgpreferences.ui \
    src/dlgreader.ui \
    src/dlgrename.ui \
    src/dlgreport.ui \
    src/dlgsettime.ui \
    src/dlgsteps.ui \
    src/dlgtodo.ui \
    src/mainwindow.ui \
    src/msgdialog.ui \
    src/onedrive/dlgweb.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    qrc.qrc \
    res.qrc

CONFIG(debug,debug|release) {
    DESTDIR = $$absolute_path($${_PRO_FILE_PWD_}/bin/debug)
} else {
    DESTDIR = $$absolute_path($${_PRO_FILE_PWD_}/bin/release)
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
    src/cn.qm \
    src/cn.ts \
    src/memo.qml \
    src/onedrive/log.qml \
    src/onedrive/main.qml \
    src/reader.qml

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android







android: include(/Users/hz/Library/Android/sdk/android_openssl/openssl.pri)
