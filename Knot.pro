QT += core gui
QT += charts sensors
QT += qml quick quickwidgets
QT += printsupport
QT += xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

ICON = res/icon.icns
TRANSLATIONS += cn.ts

unix:!macx:{
    QT += androidextras
}

#android
#{
#ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
#dataFiles.files+=src/readme.txt
#dataFiles.files+=src/1.png
#dataFiles.files+=src/2.png
#dataFiles.path = /assets/data
#INSTALLS += dataFiles
#}

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    dlglist.cpp \
    dlgmainnotes.cpp \
    dlgnotes.cpp \
    dlgpreferences.cpp \
    dlgreader.cpp \
    dlgrename.cpp \
    dlgreport.cpp \
    dlgsettime.cpp \
    dlgsteps.cpp \
    dlgtodo.cpp \
    documenthandler.cpp \
    file.cpp \
    main.cpp \
    mainwindow.cpp \
    smoothscrollbar.cpp \
    specialaccelerometerpedometer.cpp

HEADERS += \
    count_steps.h \
    dlglist.h \
    dlgmainnotes.h \
    dlgnotes.h \
    dlgpreferences.h \
    dlgreader.h \
    dlgrename.h \
    dlgreport.h \
    dlgsettime.h \
    dlgsteps.h \
    dlgtodo.h \
    documenthandler.h \
    file.h \
    mainwindow.h \
    smoothscrollbar.h \
    specialaccelerometerpedometer.h

FORMS += \
    dlglist.ui \
    dlgmainnotes.ui \
    dlgnotes.ui \
    dlgpreferences.ui \
    dlgreader.ui \
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
    android/res/values/libs.xml

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android







