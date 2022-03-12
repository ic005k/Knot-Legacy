QT       += core gui
QT       += charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

android
{
ANDROID_PACKAGE_SOURCE_DIR = $$PWD/src
dataFiles.files+=src/readme.txt
dataFiles.files+=src/Xcount.ini
dataFiles.files+=src/Xcount.txt
dataFiles.files+=src/pic.png
dataFiles.files+=src/1.png
dataFiles.files+=src/2.png
dataFiles.path = /assets/data
INSTALLS += dataFiles
}

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    chart.cpp \
    dlgnotes.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    chart.h \
    dlgnotes.h \
    mainwindow.h

FORMS += \
    dlgnotes.ui \
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
