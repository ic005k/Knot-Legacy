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
    src/EditRecord.cpp \
    src/Preferences.cpp \
    src/Reader.cpp \
    src/RemarksAbout.cpp \
    src/Report.cpp \
    src/Steps.cpp \
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
    src/dlgright.cpp \
    src/dlgsetedittext.cpp \
    src/dlgtodo.cpp \
    src/documenthandler.cpp \
    src/file.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/onedrive/qtonedrive.cpp \
    src/onedrive/qtonedriveauthorizationdialog.cpp \
    src/setTodoAlarm.cpp \
    src/specialaccelerometerpedometer.cpp

HEADERS += \
    src/EditRecord.h \
    src/Preferences.h \
    src/Reader.h \
    src/RemarksAbout.h \
    src/Report.h \
    src/Steps.h \
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
    src/dlgright.h \
    src/dlgsetedittext.h \
    src/dlgtodo.h \
    src/documenthandler.h \
    src/file.h \
    src/mainwindow.h \
    src/onedrive/qtonedrive.h \
    src/onedrive/qtonedriveauthorizationdialog.h \
    src/onedrive/qtonedrivelib_global.h \
    src/setTodoAlarm.h \
    src/specialaccelerometerpedometer.h

FORMS += \
    src/EditRecord.ui \
    src/Preferences.ui \
    src/Reader.ui \
    src/RemarksAbout.ui \
    src/Report.ui \
    src/Steps.ui \
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
    src/dlgright.ui \
    src/dlgsetedittext.ui \
    src/dlgtodo.ui \
    src/mainwindow.ui \
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
    src/qmlsrc/MainModel.qml \
    src/qmlsrc/TodoModel.qml \
    src/qmlsrc/details.qml \
    src/qmlsrc/imgview.qml \
    src/qmlsrc/main.qml \
    src/qmlsrc/memo.qml \
    src/qmlsrc/notes.qml \
    src/qmlsrc/reader.qml \
    src/qmlsrc/report.qml \
    src/qmlsrc/steps.qml \
    src/qmlsrc/todo.qml \
    src/qmlsrc/todorecycle.qml \
    src/reader.qml \
    src/steps.qml

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

#Win
#android: include(C:/Users/Administrator/AppData/Local/Android/Sdk/android_openssl/openssl.pri)
#ANDROID_EXTRA_LIBS = C:/Users/Administrator/Documents/GitHub/Knot/../../../AppData/Local/Android/Sdk/android_openssl/latest/x86_64/libcrypto_1_1.so C:/Users/Administrator/Documents/GitHub/Knot/../../../AppData/Local/Android/Sdk/android_openssl/latest/x86_64/libssl_1_1.so $$PWD/../../../AppData/Local/Android/Sdk/android_openssl/latest/arm64/libcrypto_1_1.so $$PWD/../../../AppData/Local/Android/Sdk/android_openssl/latest/arm64/libssl_1_1.so

#mac


android: include(/Users/hz/Library/Android/sdk/android_openssl/openssl.pri)

ANDROID_EXTRA_LIBS = /Users/hz/Documents/GitHub/Knot/../../../Library/Android/sdk/android_openssl/latest/x86_64/libcrypto_1_1.so /Users/hz/Documents/GitHub/Knot/../../../Library/Android/sdk/android_openssl/latest/x86_64/libssl_1_1.so $$PWD/../../../Library/Android/sdk/android_openssl/latest/arm64/libcrypto_1_1.so $$PWD/../../../Library/Android/sdk/android_openssl/latest/arm64/libssl_1_1.so
