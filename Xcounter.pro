QT       += core gui
QT       += charts sensors

#QT += qml quick
#QT += quickwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

ICON = src/icon.icns
TRANSLATIONS += cn.ts

android
{
QT += androidextras
ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
dataFiles.files+=src/readme.txt
#dataFiles.files+=src/Xcount.ini
#dataFiles.files+=src/Xcount.txt
#dataFiles.files+=src/pic.png
dataFiles.files+=src/1.png
dataFiles.files+=src/2.png
#dataFiles.path = /assets/data
INSTALLS += dataFiles
}

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    dlglist.cpp \
    dlgmainnotes.cpp \
    dlgnotes.cpp \
    dlgpreferences.cpp \
    dlgrename.cpp \
    dlgreport.cpp \
    dlgsettime.cpp \
    dlgsteps.cpp \
    dlgtodo.cpp \
    main.cpp \
    mainwindow.cpp \
    specialaccelerometerpedometer.cpp

HEADERS += \
    count_steps.h \
    dlglist.h \
    dlgmainnotes.h \
    dlgnotes.h \
    dlgpreferences.h \
    dlgrename.h \
    dlgreport.h \
    dlgsettime.h \
    dlgsteps.h \
    dlgtodo.h \
    mainwindow.h \
    specialaccelerometerpedometer.h

FORMS += \
    dlglist.ui \
    dlgmainnotes.ui \
    dlgnotes.ui \
    dlgpreferences.ui \
    dlgrename.ui \
    dlgreport.ui \
    dlgsettime.ui \
    dlgsteps.ui \
    dlgtodo.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    src.qrc

CONFIG(debug,debug|release) {
    DESTDIR = $$absolute_path($${_PRO_FILE_PWD_}/bin/debug)
} else {
    DESTDIR = $$absolute_path($${_PRO_FILE_PWD_}/bin/release)
}

DISTFILES += \
    android/AndroidManifest.xml \
    android/build.gradle \
    android/gradle.properties \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat \
    android/res/values/libs.xml \
    android/src/com/MyActivity.java \
    android/src/com/MyService.java \
    android/src/com/x/MyActivity.java \
    android/src/com/x/MyService.java \
    src/AndroidManifest.xml \
    src/build.gradle \
    src/com/android/activity/mmJavaActivity.java \
    src/gradle.properties \
    src/gradle/wrapper/gradle-wrapper.jar \
    src/gradle/wrapper/gradle-wrapper.properties \
    src/res/values/libs.xml







