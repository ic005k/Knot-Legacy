QT += core gui network printsupport core-private
QT += charts sensors
QT += qml quick quickwidgets webview positioning location
QT += xml svg

win32 {
    QMAKE_CFLAGS += /utf-8
    QMAKE_CXXFLAGS += /utf-8
}

!android: {
    QT += webenginewidgets
}

# Qt > 5 (Qt6)
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat statemachine
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# 添加concurrent模块支持
QT += concurrent

CONFIG += c++17
CONFIG+=sdk_no_version_check

TRANSLATIONS += src/cn.ts

ICON = res/icon.icns
RC_FILE +=win.rc

include($$PWD/src/3rdparty/zlib.pri)

android: {
    lessThan(QT_MAJOR_VERSION, 6):QT += androidextras

}

# 添加 cppjieba 和 limonp 头文件路径
INCLUDEPATH += $$PWD/cppjieba/include
INCLUDEPATH += $$PWD/cppjieba/limonp/include

############################ cmark-gfm ########################################

INCLUDEPATH += $$PWD/lib/cmark-gfm/include

# 强制单线程链接
#QMAKE_CXXFLAGS += -pthread
#QMAKE_LFLAGS += -Wl,--no-as-needed -pthread

win32:{
# 静态库路径（Windows 示例）
# Windows MSVC
    #QMAKE_CXXFLAGS_RELEASE += /MT
    #LIBS += $$PWD/lib/cmark-gfm/cmark-gfm_static.lib
    #LIBS += $$PWD/lib/cmark-gfm/cmark-gfm-extensions_static.lib
    # 必须链接 Windows 系统库
    #LIBS += -ladvapi32 -luserenv
}
# Linux额外依赖
unix:!macx: {
    #LIBS += $$PWD/lib/cmark-gfm/linux/libcmark-gfm.a
    #LIBS += -lpthread
}

#################################################################################


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#DEFINES += QT_DEPRECATED_WARNINGS \
#           QT_ANGLE_PLATFORM

SOURCES += \
    src/AboutThis.cpp \
    src/AutoUpdate.cpp \
    src/CategoryList.cpp \
    src/CloudBackup.cpp \
    src/Comm/FileSystemWatcher.cpp \
    src/Comm/Method.cpp \
    src/Comm/ReceiveShare.cpp \
    src/Comm/RollingBox.cpp \
    src/Comm/ShowMessage.cpp \
    src/Comm/qaesencryption.cpp \
    src/Comm/qzipfile.cpp \
    src/Comm/qzipfileentry.cpp \
    src/DateSelector.cpp \
    src/EditRecord.cpp \
    src/LoadPic.cpp \
    src/MainWindow.cpp \
    src/Notes/ColorDialog.cpp \
    src/Notes/LineNumberArea.cpp \
    src/Notes/MarkdownHighlighter.cpp \
    src/Notes/MoveTo.cpp \
    src/Notes/NewNoteBook.cpp \
    src/Notes/Notes.cpp \
    src/Notes/NotesList.cpp \
    src/Notes/NotesSearchEngine.cpp \
    src/Notes/PrintPDF.cpp \
    src/Notes/QOwnLanguageData.cpp \
    src/Notes/QTextEditHighlighter.cpp \
    src/Notes/SearchResultModel.cpp \
    src/Notes/TextSelector.cpp \
    src/Preferences.cpp \
    src/Reader/DocumentHandler.cpp \
    src/Reader/File.cpp \
    src/Reader/PageIndicator.cpp \
    src/Reader/Reader.cpp \
    src/Reader/ReaderSet.cpp \
    src/Reader/SetReaderText.cpp \
    src/Report.cpp \
    src/Steps/SpecialAccelerometerPedometer.cpp \
    src/Steps/Steps.cpp \
    src/Steps/StepsOptions.cpp \
    src/SyncInfo.cpp \
    src/Todo/Todo.cpp \
    src/Todo/TodoAlarm.cpp \
    src/cmark-gfm/extensions/autolink.c \
    src/cmark-gfm/extensions/core-extensions.c \
    src/cmark-gfm/extensions/ext_scanners.c \
    src/cmark-gfm/extensions/strikethrough.c \
    src/cmark-gfm/extensions/table.c \
    src/cmark-gfm/extensions/tagfilter.c \
    src/cmark-gfm/extensions/tasklist.c \
    src/cmark-gfm/src/arena.c \
    src/cmark-gfm/src/blocks.c \
    src/cmark-gfm/src/buffer.c \
    src/cmark-gfm/src/cmark.c \
    src/cmark-gfm/src/cmark_ctype.c \
    src/cmark-gfm/src/commonmark.c \
    src/cmark-gfm/src/footnotes.c \
    src/cmark-gfm/src/houdini_href_e.c \
    src/cmark-gfm/src/houdini_html_e.c \
    src/cmark-gfm/src/houdini_html_u.c \
    src/cmark-gfm/src/html.c \
    src/cmark-gfm/src/inlines.c \
    src/cmark-gfm/src/iterator.c \
    src/cmark-gfm/src/latex.c \
    src/cmark-gfm/src/linked_list.c \
    src/cmark-gfm/src/man.c \
    src/cmark-gfm/src/map.c \
    src/cmark-gfm/src/node.c \
    src/cmark-gfm/src/plaintext.c \
    src/cmark-gfm/src/plugin.c \
    src/cmark-gfm/src/references.c \
    src/cmark-gfm/src/registry.c \
    src/cmark-gfm/src/render.c \
    src/cmark-gfm/src/scanners.c \
    src/cmark-gfm/src/syntax_extension.c \
    src/cmark-gfm/src/utf8.c \
    src/cmark-gfm/src/xml.c \
    src/main.cpp \
    src/md4c/entity.c \
    src/md4c/md4c-html.c \
    src/md4c/md4c.c \
    src/onedrive/qtonedrive.cpp \
    src/onedrive/qtonedriveauthorizationdialog.cpp \
    src/onedrive/qtonedrivewebview.cpp


HEADERS += \
    src/AboutThis.h \
    src/AutoUpdate.h \
    src/CategoryList.h \
    src/CloudBackup.h \
    src/Comm/FileSystemWatcher.h \
    src/Comm/Method.h \
    src/Comm/ReceiveShare.h \
    src/Comm/RollingBox.h \
    src/Comm/ShowMessage.h \
    src/Comm/qaesencryption.h \
    src/Comm/qzipfile.h \
    src/Comm/qzipfileentry.h \
    src/DateSelector.h \
    src/EditRecord.h \
    src/LoadPic.h \
    src/MainWindow.h \
    src/Notes/ColorDialog.h \
    src/Notes/LineNumberArea.h \
    src/Notes/MarkdownHighlighter.h \
    src/Notes/MoveTo.h \
    src/Notes/NewNoteBook.h \
    src/Notes/Notes.h \
    src/Notes/NotesList.h \
    src/Notes/NotesSearchEngine.h \
    src/Notes/PrintPDF.h \
    src/Notes/QOwnLanguageData.h \
    src/Notes/QTextEditHighlighter.h \
    src/Notes/SearchResultModel.h \
    src/Notes/TextSelector.h \
    src/Preferences.h \
    src/Reader/DocumentHandler.h \
    src/Reader/File.h \
    src/Reader/PageIndicator.h \
    src/Reader/Reader.h \
    src/Reader/ReaderSet.h \
    src/Reader/SetReaderText.h \
    src/Report.h \
    src/Steps/SpecialAccelerometerPedometer.h \
    src/Steps/Steps.h \
    src/Steps/StepsOptions.h \
    src/SyncInfo.h \
    src/Todo/Todo.h \
    src/Todo/TodoAlarm.h \
    src/md4c/entity.h \
    src/md4c/md4c-html.h \
    src/md4c/md4c.h \
    src/onedrive/qtonedrive.h \
    src/onedrive/qtonedriveauthorizationdialog.h \
    src/onedrive/qtonedrivelib_global.h \
    src/onedrive/qtonedrivewebview.h \
    win.rc

FORMS += \
    src/AboutThis.ui \
    src/AutoUpdate.ui \
    src/CategoryList.ui \
    src/CloudBackup.ui \
    src/Comm/ShowMessage.ui \
    src/DateSelector.ui \
    src/MainWindow.ui \
    src/Notes/MoveTo.ui \
    src/Notes/NewNoteBook.ui \
    src/Notes/Notes.ui \
    src/Notes/NotesList.ui \
    src/Notes/PrintPDF.ui \
    src/Notes/TextSelector.ui \
    src/Preferences.ui \
    src/Reader/PageIndicator.ui \
    src/Reader/SetReaderText.ui \
    src/Steps/StepsOptions.ui \
    src/SyncInfo.ui \
    src/Todo/Todo.ui \
    src/Todo/TodoAlarm.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    knotqml.qrc \
    res.qrc \


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
    android/res/values/libs.xml \
    android/res/xml/qtprovider_paths.xml \
    src/3rdparty/zlib.pri \
    src/cmark-gfm/src/case_fold_switch.inc \
    src/cmark-gfm/src/entities.inc \
    src/cn.qm \
    src/cn.ts \
    src/imgview.qml \
    src/memo.qml \
    src/onedrive/log.qml \
    src/onedrive/web.qml \
    src/qmlsrc/MainModel.qml \
    src/qmlsrc/TodoModel.qml \
    src/qmlsrc/TreeItem.js \
    src/qmlsrc/TreeWidget.qml \
    src/qmlsrc/baklist.qml \
    src/qmlsrc/booklist.qml \
    src/qmlsrc/bookmark.qml \
    src/qmlsrc/bookview.qml \
    src/qmlsrc/details.qml \
    src/qmlsrc/epub_cata.qml \
    src/qmlsrc/gps_list.qml \
    src/qmlsrc/imgview.qml \
    src/qmlsrc/main.qml \
    src/qmlsrc/maindate.qml \
    src/qmlsrc/mainevent.qml \
    src/qmlsrc/maintab.qml \
    src/qmlsrc/map.qml \
    src/qmlsrc/memo.qml \
    src/qmlsrc/notebook.qml \
    src/qmlsrc/noteeditor.qml \
    src/qmlsrc/notelist.qml \
    src/qmlsrc/noterecycle.qml \
    src/qmlsrc/notes.qml \
    src/qmlsrc/notes_search_result.qml \
    src/qmlsrc/pdf.qml \
    src/qmlsrc/reader.qml \
    src/qmlsrc/report.qml \
    src/qmlsrc/search.qml \
    src/qmlsrc/seltab.qml \
    src/qmlsrc/steps.qml \
    src/qmlsrc/tabrecycle.qml \
    src/qmlsrc/todo.qml \
    src/qmlsrc/todorecycle.qml \
    src/qmlsrc/tree_main.qml \
    src/qmlsrc/type.qml \
    src/qmlsrc/viewcate.qml \
    src/qzip/zlib.pri \
    src/reader.qml \
    src/steps.qml



ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

######################### OpenSSL #####################################################
#Linux
unix:!macx: {
    android: include(/home/zh/文档/android_openssl-master/openssl.pri)
}

# android: include($$PWD/android_openssl-master/openssl.pri)
# ANDROID_EXTRA_LIBS = $$PWD/android_openssl-master/ssl_1.1/arm64-v8a/libcrypto_1_1.so $$PWD/android_openssl-master/ssl_1.1/arm64-v8a/libssl_1_1.so

win32:{
    android: include(C:/Users/Administrator/Documents/android_openssl-master/openssl.pri)
}

ANDROID_EXTRA_LIBS = $$PWD/android-openssl/libcrypto_1_1.so $$PWD/android-openssl/libssl_1_1.so


