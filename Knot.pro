QT += core gui network
QT += charts sensors
QT += qml quick quickwidgets webview
QT += xml svg

!android: {
    QT += webenginewidgets
}

# Qt > 5 (Qt6)
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
CONFIG+=sdk_no_version_check

TRANSLATIONS += src/cn.ts

ICON = res/icon.icns
RC_FILE +=win.rc

# Pdf View library
QML_IMPORT_PATH += $$PWD/
QML_IMPORT_PATH += $$PWD/libs/qt-pdf-viewer-library/
QML_IMPORT_PATH += $$PWD/libs/qt-pdf-viewer-library/it/ltdev/qt/qml/components/
QML_DESIGNER_IMPORT_PATH += $$PWD/
QML_DESIGNER_IMPORT_PATH += $$PWD/libs/qt-pdf-viewer-library/
QML_DESIGNER_IMPORT_PATH += $$PWD/libs/qt-pdf-viewer-library/it/ltdev/qt/qml/components/
include($$PWD/libs/qt-pdf-viewer-library/qtpdfviewer.pri)

unix:!macx:{

}

android: {
    lessThan(QT_MAJOR_VERSION, 6):QT += androidextras
}


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/EditRecord.cpp \
    src/Notes.cpp \
    src/NotesList.cpp \
    src/OneDrive.cpp \
    src/Preferences.cpp \
    src/Reader.cpp \
    src/ReaderFun.cpp \
    src/RemarksAbout.cpp \
    src/Report.cpp \
    src/Steps.cpp \
    src/Todo.cpp \
    src/TodoAlarm.cpp \
    src/autoupdatedialog.cpp \
    src/dlgSetText.cpp \
    src/dlgfloatfun.cpp \
    src/dlgleft.cpp \
    src/dlglist.cpp \
    src/dlgloadpic.cpp \
    src/dlgright.cpp \
    src/dlgsetedittext.cpp \
    src/documenthandler.cpp \
    src/file.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/onedrive/qtonedrive.cpp \
    src/onedrive/qtonedriveauthorizationdialog.cpp \
    src/onedrive/qtonedrivewebview.cpp \
    src/specialaccelerometerpedometer.cpp

HEADERS += \
    src/EditRecord.h \
    src/Notes.h \
    src/NotesList.h \
    src/OneDrive.h \
    src/Preferences.h \
    src/Reader.h \
    src/ReaderFun.h \
    src/RemarksAbout.h \
    src/Report.h \
    src/Steps.h \
    src/Todo.h \
    src/TodoAlarm.h \
    src/autoupdatedialog.h \
    src/count_steps.h \
    src/dlgSetText.h \
    src/dlgfloatfun.h \
    src/dlgleft.h \
    src/dlglist.h \
    src/dlgloadpic.h \
    src/dlgright.h \
    src/dlgsetedittext.h \
    src/documenthandler.h \
    src/file.h \
    src/mainwindow.h \
    src/onedrive/qtonedrive.h \
    src/onedrive/qtonedriveauthorizationdialog.h \
    src/onedrive/qtonedrivelib_global.h \
    src/onedrive/qtonedrivewebview.h \
    src/specialaccelerometerpedometer.h \
    win.rc

FORMS += \
    src/EditRecord.ui \
    src/Notes.ui \
    src/NotesList.ui \
    src/OneDrive.ui \
    src/Preferences.ui \
    src/Reader.ui \
    src/ReaderFun.ui \
    src/RemarksAbout.ui \
    src/Report.ui \
    src/Steps.ui \
    src/Todo.ui \
    src/TodoAlarm.ui \
    src/autoupdatedialog.ui \
    src/dlgSetText.ui \
    src/dlgfloatfun.ui \
    src/dlgleft.ui \
    src/dlglist.ui \
    src/dlgloadpic.ui \
    src/dlgright.ui \
    src/dlgsetedittext.ui \
    src/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icons.qrc \
    knotqml.qrc \
    pdfqml.qrc \
    res.qrc

CONFIG(debug,debug|release) {
    DESTDIR = $$absolute_path($${_PRO_FILE_PWD_}/bin/debug)
} else {
    DESTDIR = $$absolute_path($${_PRO_FILE_PWD_}/bin/release)
}

DISTFILES += \
    android/AndroidManifest.xml \
    android/assets/pdfjs/build/pdf.js \
    android/assets/pdfjs/build/pdf.js.map \
    android/assets/pdfjs/build/pdf.worker.js \
    android/assets/pdfjs/build/pdf.worker.js.map \
    android/assets/pdfjs/debugger.js \
    android/assets/pdfjs/pdf.js \
    android/assets/pdfjs/pdf.viewer.bridge.js \
    android/assets/pdfjs/pdf.viewer.css \
    android/assets/pdfjs/pdf.worker.js \
    android/assets/pdfjs/qwebchannel.js \
    android/assets/pdfjs/viewer.css \
    android/assets/pdfjs/viewer.html \
    android/assets/pdfjs/viewer.js \
    android/assets/pdfjs/web/cmaps/78-EUC-H.bcmap \
    android/assets/pdfjs/web/cmaps/78-EUC-V.bcmap \
    android/assets/pdfjs/web/cmaps/78-H.bcmap \
    android/assets/pdfjs/web/cmaps/78-RKSJ-H.bcmap \
    android/assets/pdfjs/web/cmaps/78-RKSJ-V.bcmap \
    android/assets/pdfjs/web/cmaps/78-V.bcmap \
    android/assets/pdfjs/web/cmaps/78ms-RKSJ-H.bcmap \
    android/assets/pdfjs/web/cmaps/78ms-RKSJ-V.bcmap \
    android/assets/pdfjs/web/cmaps/83pv-RKSJ-H.bcmap \
    android/assets/pdfjs/web/cmaps/90ms-RKSJ-H.bcmap \
    android/assets/pdfjs/web/cmaps/90ms-RKSJ-V.bcmap \
    android/assets/pdfjs/web/cmaps/90msp-RKSJ-H.bcmap \
    android/assets/pdfjs/web/cmaps/90msp-RKSJ-V.bcmap \
    android/assets/pdfjs/web/cmaps/90pv-RKSJ-H.bcmap \
    android/assets/pdfjs/web/cmaps/90pv-RKSJ-V.bcmap \
    android/assets/pdfjs/web/cmaps/Add-H.bcmap \
    android/assets/pdfjs/web/cmaps/Add-RKSJ-H.bcmap \
    android/assets/pdfjs/web/cmaps/Add-RKSJ-V.bcmap \
    android/assets/pdfjs/web/cmaps/Add-V.bcmap \
    android/assets/pdfjs/web/cmaps/Adobe-CNS1-0.bcmap \
    android/assets/pdfjs/web/cmaps/Adobe-CNS1-1.bcmap \
    android/assets/pdfjs/web/cmaps/Adobe-CNS1-2.bcmap \
    android/assets/pdfjs/web/cmaps/Adobe-CNS1-3.bcmap \
    android/assets/pdfjs/web/cmaps/Adobe-CNS1-4.bcmap \
    android/assets/pdfjs/web/cmaps/Adobe-CNS1-5.bcmap \
    android/assets/pdfjs/web/cmaps/Adobe-CNS1-6.bcmap \
    android/assets/pdfjs/web/cmaps/Adobe-CNS1-UCS2.bcmap \
    android/assets/pdfjs/web/cmaps/Adobe-GB1-0.bcmap \
    android/assets/pdfjs/web/cmaps/Adobe-GB1-1.bcmap \
    android/assets/pdfjs/web/cmaps/Adobe-GB1-2.bcmap \
    android/assets/pdfjs/web/cmaps/Adobe-GB1-3.bcmap \
    android/assets/pdfjs/web/cmaps/Adobe-GB1-4.bcmap \
    android/assets/pdfjs/web/cmaps/Adobe-GB1-5.bcmap \
    android/assets/pdfjs/web/cmaps/Adobe-GB1-UCS2.bcmap \
    android/assets/pdfjs/web/cmaps/Adobe-Japan1-0.bcmap \
    android/assets/pdfjs/web/cmaps/Adobe-Japan1-1.bcmap \
    android/assets/pdfjs/web/cmaps/Adobe-Japan1-2.bcmap \
    android/assets/pdfjs/web/cmaps/Adobe-Japan1-3.bcmap \
    android/assets/pdfjs/web/cmaps/Adobe-Japan1-4.bcmap \
    android/assets/pdfjs/web/cmaps/Adobe-Japan1-5.bcmap \
    android/assets/pdfjs/web/cmaps/Adobe-Japan1-6.bcmap \
    android/assets/pdfjs/web/cmaps/Adobe-Japan1-UCS2.bcmap \
    android/assets/pdfjs/web/cmaps/Adobe-Korea1-0.bcmap \
    android/assets/pdfjs/web/cmaps/Adobe-Korea1-1.bcmap \
    android/assets/pdfjs/web/cmaps/Adobe-Korea1-2.bcmap \
    android/assets/pdfjs/web/cmaps/Adobe-Korea1-UCS2.bcmap \
    android/assets/pdfjs/web/cmaps/B5-H.bcmap \
    android/assets/pdfjs/web/cmaps/B5-V.bcmap \
    android/assets/pdfjs/web/cmaps/B5pc-H.bcmap \
    android/assets/pdfjs/web/cmaps/B5pc-V.bcmap \
    android/assets/pdfjs/web/cmaps/CNS-EUC-H.bcmap \
    android/assets/pdfjs/web/cmaps/CNS-EUC-V.bcmap \
    android/assets/pdfjs/web/cmaps/CNS1-H.bcmap \
    android/assets/pdfjs/web/cmaps/CNS1-V.bcmap \
    android/assets/pdfjs/web/cmaps/CNS2-H.bcmap \
    android/assets/pdfjs/web/cmaps/CNS2-V.bcmap \
    android/assets/pdfjs/web/cmaps/ETHK-B5-H.bcmap \
    android/assets/pdfjs/web/cmaps/ETHK-B5-V.bcmap \
    android/assets/pdfjs/web/cmaps/ETen-B5-H.bcmap \
    android/assets/pdfjs/web/cmaps/ETen-B5-V.bcmap \
    android/assets/pdfjs/web/cmaps/ETenms-B5-H.bcmap \
    android/assets/pdfjs/web/cmaps/ETenms-B5-V.bcmap \
    android/assets/pdfjs/web/cmaps/EUC-H.bcmap \
    android/assets/pdfjs/web/cmaps/EUC-V.bcmap \
    android/assets/pdfjs/web/cmaps/Ext-H.bcmap \
    android/assets/pdfjs/web/cmaps/Ext-RKSJ-H.bcmap \
    android/assets/pdfjs/web/cmaps/Ext-RKSJ-V.bcmap \
    android/assets/pdfjs/web/cmaps/Ext-V.bcmap \
    android/assets/pdfjs/web/cmaps/GB-EUC-H.bcmap \
    android/assets/pdfjs/web/cmaps/GB-EUC-V.bcmap \
    android/assets/pdfjs/web/cmaps/GB-H.bcmap \
    android/assets/pdfjs/web/cmaps/GB-V.bcmap \
    android/assets/pdfjs/web/cmaps/GBK-EUC-H.bcmap \
    android/assets/pdfjs/web/cmaps/GBK-EUC-V.bcmap \
    android/assets/pdfjs/web/cmaps/GBK2K-H.bcmap \
    android/assets/pdfjs/web/cmaps/GBK2K-V.bcmap \
    android/assets/pdfjs/web/cmaps/GBKp-EUC-H.bcmap \
    android/assets/pdfjs/web/cmaps/GBKp-EUC-V.bcmap \
    android/assets/pdfjs/web/cmaps/GBT-EUC-H.bcmap \
    android/assets/pdfjs/web/cmaps/GBT-EUC-V.bcmap \
    android/assets/pdfjs/web/cmaps/GBT-H.bcmap \
    android/assets/pdfjs/web/cmaps/GBT-V.bcmap \
    android/assets/pdfjs/web/cmaps/GBTpc-EUC-H.bcmap \
    android/assets/pdfjs/web/cmaps/GBTpc-EUC-V.bcmap \
    android/assets/pdfjs/web/cmaps/GBpc-EUC-H.bcmap \
    android/assets/pdfjs/web/cmaps/GBpc-EUC-V.bcmap \
    android/assets/pdfjs/web/cmaps/H.bcmap \
    android/assets/pdfjs/web/cmaps/HKdla-B5-H.bcmap \
    android/assets/pdfjs/web/cmaps/HKdla-B5-V.bcmap \
    android/assets/pdfjs/web/cmaps/HKdlb-B5-H.bcmap \
    android/assets/pdfjs/web/cmaps/HKdlb-B5-V.bcmap \
    android/assets/pdfjs/web/cmaps/HKgccs-B5-H.bcmap \
    android/assets/pdfjs/web/cmaps/HKgccs-B5-V.bcmap \
    android/assets/pdfjs/web/cmaps/HKm314-B5-H.bcmap \
    android/assets/pdfjs/web/cmaps/HKm314-B5-V.bcmap \
    android/assets/pdfjs/web/cmaps/HKm471-B5-H.bcmap \
    android/assets/pdfjs/web/cmaps/HKm471-B5-V.bcmap \
    android/assets/pdfjs/web/cmaps/HKscs-B5-H.bcmap \
    android/assets/pdfjs/web/cmaps/HKscs-B5-V.bcmap \
    android/assets/pdfjs/web/cmaps/Hankaku.bcmap \
    android/assets/pdfjs/web/cmaps/Hiragana.bcmap \
    android/assets/pdfjs/web/cmaps/KSC-EUC-H.bcmap \
    android/assets/pdfjs/web/cmaps/KSC-EUC-V.bcmap \
    android/assets/pdfjs/web/cmaps/KSC-H.bcmap \
    android/assets/pdfjs/web/cmaps/KSC-Johab-H.bcmap \
    android/assets/pdfjs/web/cmaps/KSC-Johab-V.bcmap \
    android/assets/pdfjs/web/cmaps/KSC-V.bcmap \
    android/assets/pdfjs/web/cmaps/KSCms-UHC-H.bcmap \
    android/assets/pdfjs/web/cmaps/KSCms-UHC-HW-H.bcmap \
    android/assets/pdfjs/web/cmaps/KSCms-UHC-HW-V.bcmap \
    android/assets/pdfjs/web/cmaps/KSCms-UHC-V.bcmap \
    android/assets/pdfjs/web/cmaps/KSCpc-EUC-H.bcmap \
    android/assets/pdfjs/web/cmaps/KSCpc-EUC-V.bcmap \
    android/assets/pdfjs/web/cmaps/Katakana.bcmap \
    android/assets/pdfjs/web/cmaps/LICENSE \
    android/assets/pdfjs/web/cmaps/NWP-H.bcmap \
    android/assets/pdfjs/web/cmaps/NWP-V.bcmap \
    android/assets/pdfjs/web/cmaps/RKSJ-H.bcmap \
    android/assets/pdfjs/web/cmaps/RKSJ-V.bcmap \
    android/assets/pdfjs/web/cmaps/Roman.bcmap \
    android/assets/pdfjs/web/cmaps/UniCNS-UCS2-H.bcmap \
    android/assets/pdfjs/web/cmaps/UniCNS-UCS2-V.bcmap \
    android/assets/pdfjs/web/cmaps/UniCNS-UTF16-H.bcmap \
    android/assets/pdfjs/web/cmaps/UniCNS-UTF16-V.bcmap \
    android/assets/pdfjs/web/cmaps/UniCNS-UTF32-H.bcmap \
    android/assets/pdfjs/web/cmaps/UniCNS-UTF32-V.bcmap \
    android/assets/pdfjs/web/cmaps/UniCNS-UTF8-H.bcmap \
    android/assets/pdfjs/web/cmaps/UniCNS-UTF8-V.bcmap \
    android/assets/pdfjs/web/cmaps/UniGB-UCS2-H.bcmap \
    android/assets/pdfjs/web/cmaps/UniGB-UCS2-V.bcmap \
    android/assets/pdfjs/web/cmaps/UniGB-UTF16-H.bcmap \
    android/assets/pdfjs/web/cmaps/UniGB-UTF16-V.bcmap \
    android/assets/pdfjs/web/cmaps/UniGB-UTF32-H.bcmap \
    android/assets/pdfjs/web/cmaps/UniGB-UTF32-V.bcmap \
    android/assets/pdfjs/web/cmaps/UniGB-UTF8-H.bcmap \
    android/assets/pdfjs/web/cmaps/UniGB-UTF8-V.bcmap \
    android/assets/pdfjs/web/cmaps/UniJIS-UCS2-H.bcmap \
    android/assets/pdfjs/web/cmaps/UniJIS-UCS2-HW-H.bcmap \
    android/assets/pdfjs/web/cmaps/UniJIS-UCS2-HW-V.bcmap \
    android/assets/pdfjs/web/cmaps/UniJIS-UCS2-V.bcmap \
    android/assets/pdfjs/web/cmaps/UniJIS-UTF16-H.bcmap \
    android/assets/pdfjs/web/cmaps/UniJIS-UTF16-V.bcmap \
    android/assets/pdfjs/web/cmaps/UniJIS-UTF32-H.bcmap \
    android/assets/pdfjs/web/cmaps/UniJIS-UTF32-V.bcmap \
    android/assets/pdfjs/web/cmaps/UniJIS-UTF8-H.bcmap \
    android/assets/pdfjs/web/cmaps/UniJIS-UTF8-V.bcmap \
    android/assets/pdfjs/web/cmaps/UniJIS2004-UTF16-H.bcmap \
    android/assets/pdfjs/web/cmaps/UniJIS2004-UTF16-V.bcmap \
    android/assets/pdfjs/web/cmaps/UniJIS2004-UTF32-H.bcmap \
    android/assets/pdfjs/web/cmaps/UniJIS2004-UTF32-V.bcmap \
    android/assets/pdfjs/web/cmaps/UniJIS2004-UTF8-H.bcmap \
    android/assets/pdfjs/web/cmaps/UniJIS2004-UTF8-V.bcmap \
    android/assets/pdfjs/web/cmaps/UniJISPro-UCS2-HW-V.bcmap \
    android/assets/pdfjs/web/cmaps/UniJISPro-UCS2-V.bcmap \
    android/assets/pdfjs/web/cmaps/UniJISPro-UTF8-V.bcmap \
    android/assets/pdfjs/web/cmaps/UniJISX0213-UTF32-H.bcmap \
    android/assets/pdfjs/web/cmaps/UniJISX0213-UTF32-V.bcmap \
    android/assets/pdfjs/web/cmaps/UniJISX02132004-UTF32-H.bcmap \
    android/assets/pdfjs/web/cmaps/UniJISX02132004-UTF32-V.bcmap \
    android/assets/pdfjs/web/cmaps/UniKS-UCS2-H.bcmap \
    android/assets/pdfjs/web/cmaps/UniKS-UCS2-V.bcmap \
    android/assets/pdfjs/web/cmaps/UniKS-UTF16-H.bcmap \
    android/assets/pdfjs/web/cmaps/UniKS-UTF16-V.bcmap \
    android/assets/pdfjs/web/cmaps/UniKS-UTF32-H.bcmap \
    android/assets/pdfjs/web/cmaps/UniKS-UTF32-V.bcmap \
    android/assets/pdfjs/web/cmaps/UniKS-UTF8-H.bcmap \
    android/assets/pdfjs/web/cmaps/UniKS-UTF8-V.bcmap \
    android/assets/pdfjs/web/cmaps/V.bcmap \
    android/assets/pdfjs/web/cmaps/WP-Symbol.bcmap \
    android/assets/pdfjs/web/compressed.tracemonkey-pldi-09.pdf \
    android/assets/pdfjs/web/debugger.js \
    android/assets/pdfjs/web/images/annotation-check.svg \
    android/assets/pdfjs/web/images/annotation-comment.svg \
    android/assets/pdfjs/web/images/annotation-help.svg \
    android/assets/pdfjs/web/images/annotation-insert.svg \
    android/assets/pdfjs/web/images/annotation-key.svg \
    android/assets/pdfjs/web/images/annotation-newparagraph.svg \
    android/assets/pdfjs/web/images/annotation-noicon.svg \
    android/assets/pdfjs/web/images/annotation-note.svg \
    android/assets/pdfjs/web/images/annotation-paragraph.svg \
    android/assets/pdfjs/web/images/findbarButton-next-rtl.png \
    android/assets/pdfjs/web/images/findbarButton-next-rtl@2x.png \
    android/assets/pdfjs/web/images/findbarButton-next.png \
    android/assets/pdfjs/web/images/findbarButton-next@2x.png \
    android/assets/pdfjs/web/images/findbarButton-previous-rtl.png \
    android/assets/pdfjs/web/images/findbarButton-previous-rtl@2x.png \
    android/assets/pdfjs/web/images/findbarButton-previous.png \
    android/assets/pdfjs/web/images/findbarButton-previous@2x.png \
    android/assets/pdfjs/web/images/grab.cur \
    android/assets/pdfjs/web/images/grabbing.cur \
    android/assets/pdfjs/web/images/loading-icon.gif \
    android/assets/pdfjs/web/images/loading-small.png \
    android/assets/pdfjs/web/images/loading-small@2x.png \
    android/assets/pdfjs/web/images/secondaryToolbarButton-documentProperties.png \
    android/assets/pdfjs/web/images/secondaryToolbarButton-documentProperties@2x.png \
    android/assets/pdfjs/web/images/secondaryToolbarButton-firstPage.png \
    android/assets/pdfjs/web/images/secondaryToolbarButton-firstPage@2x.png \
    android/assets/pdfjs/web/images/secondaryToolbarButton-handTool.png \
    android/assets/pdfjs/web/images/secondaryToolbarButton-handTool@2x.png \
    android/assets/pdfjs/web/images/secondaryToolbarButton-lastPage.png \
    android/assets/pdfjs/web/images/secondaryToolbarButton-lastPage@2x.png \
    android/assets/pdfjs/web/images/secondaryToolbarButton-rotateCcw.png \
    android/assets/pdfjs/web/images/secondaryToolbarButton-rotateCcw@2x.png \
    android/assets/pdfjs/web/images/secondaryToolbarButton-rotateCw.png \
    android/assets/pdfjs/web/images/secondaryToolbarButton-rotateCw@2x.png \
    android/assets/pdfjs/web/images/secondaryToolbarButton-scrollHorizontal.png \
    android/assets/pdfjs/web/images/secondaryToolbarButton-scrollHorizontal@2x.png \
    android/assets/pdfjs/web/images/secondaryToolbarButton-scrollVertical.png \
    android/assets/pdfjs/web/images/secondaryToolbarButton-scrollVertical@2x.png \
    android/assets/pdfjs/web/images/secondaryToolbarButton-scrollWrapped.png \
    android/assets/pdfjs/web/images/secondaryToolbarButton-scrollWrapped@2x.png \
    android/assets/pdfjs/web/images/secondaryToolbarButton-selectTool.png \
    android/assets/pdfjs/web/images/secondaryToolbarButton-selectTool@2x.png \
    android/assets/pdfjs/web/images/secondaryToolbarButton-spreadEven.png \
    android/assets/pdfjs/web/images/secondaryToolbarButton-spreadEven@2x.png \
    android/assets/pdfjs/web/images/secondaryToolbarButton-spreadNone.png \
    android/assets/pdfjs/web/images/secondaryToolbarButton-spreadNone@2x.png \
    android/assets/pdfjs/web/images/secondaryToolbarButton-spreadOdd.png \
    android/assets/pdfjs/web/images/secondaryToolbarButton-spreadOdd@2x.png \
    android/assets/pdfjs/web/images/shadow.png \
    android/assets/pdfjs/web/images/texture.png \
    android/assets/pdfjs/web/images/toolbarButton-bookmark.png \
    android/assets/pdfjs/web/images/toolbarButton-bookmark@2x.png \
    android/assets/pdfjs/web/images/toolbarButton-download.png \
    android/assets/pdfjs/web/images/toolbarButton-download@2x.png \
    android/assets/pdfjs/web/images/toolbarButton-menuArrows.png \
    android/assets/pdfjs/web/images/toolbarButton-menuArrows@2x.png \
    android/assets/pdfjs/web/images/toolbarButton-openFile.png \
    android/assets/pdfjs/web/images/toolbarButton-openFile@2x.png \
    android/assets/pdfjs/web/images/toolbarButton-pageDown-rtl.png \
    android/assets/pdfjs/web/images/toolbarButton-pageDown-rtl@2x.png \
    android/assets/pdfjs/web/images/toolbarButton-pageDown.png \
    android/assets/pdfjs/web/images/toolbarButton-pageDown@2x.png \
    android/assets/pdfjs/web/images/toolbarButton-pageUp-rtl.png \
    android/assets/pdfjs/web/images/toolbarButton-pageUp-rtl@2x.png \
    android/assets/pdfjs/web/images/toolbarButton-pageUp.png \
    android/assets/pdfjs/web/images/toolbarButton-pageUp@2x.png \
    android/assets/pdfjs/web/images/toolbarButton-presentationMode.png \
    android/assets/pdfjs/web/images/toolbarButton-presentationMode@2x.png \
    android/assets/pdfjs/web/images/toolbarButton-print.png \
    android/assets/pdfjs/web/images/toolbarButton-print@2x.png \
    android/assets/pdfjs/web/images/toolbarButton-search.png \
    android/assets/pdfjs/web/images/toolbarButton-search@2x.png \
    android/assets/pdfjs/web/images/toolbarButton-secondaryToolbarToggle-rtl.png \
    android/assets/pdfjs/web/images/toolbarButton-secondaryToolbarToggle-rtl@2x.png \
    android/assets/pdfjs/web/images/toolbarButton-secondaryToolbarToggle.png \
    android/assets/pdfjs/web/images/toolbarButton-secondaryToolbarToggle@2x.png \
    android/assets/pdfjs/web/images/toolbarButton-sidebarToggle-rtl.png \
    android/assets/pdfjs/web/images/toolbarButton-sidebarToggle-rtl@2x.png \
    android/assets/pdfjs/web/images/toolbarButton-sidebarToggle.png \
    android/assets/pdfjs/web/images/toolbarButton-sidebarToggle@2x.png \
    android/assets/pdfjs/web/images/toolbarButton-viewAttachments.png \
    android/assets/pdfjs/web/images/toolbarButton-viewAttachments@2x.png \
    android/assets/pdfjs/web/images/toolbarButton-viewOutline-rtl.png \
    android/assets/pdfjs/web/images/toolbarButton-viewOutline-rtl@2x.png \
    android/assets/pdfjs/web/images/toolbarButton-viewOutline.png \
    android/assets/pdfjs/web/images/toolbarButton-viewOutline@2x.png \
    android/assets/pdfjs/web/images/toolbarButton-viewThumbnail.png \
    android/assets/pdfjs/web/images/toolbarButton-viewThumbnail@2x.png \
    android/assets/pdfjs/web/images/toolbarButton-zoomIn.png \
    android/assets/pdfjs/web/images/toolbarButton-zoomIn@2x.png \
    android/assets/pdfjs/web/images/toolbarButton-zoomOut.png \
    android/assets/pdfjs/web/images/toolbarButton-zoomOut@2x.png \
    android/assets/pdfjs/web/images/treeitem-collapsed-rtl.png \
    android/assets/pdfjs/web/images/treeitem-collapsed-rtl@2x.png \
    android/assets/pdfjs/web/images/treeitem-collapsed.png \
    android/assets/pdfjs/web/images/treeitem-collapsed@2x.png \
    android/assets/pdfjs/web/images/treeitem-expanded.png \
    android/assets/pdfjs/web/images/treeitem-expanded@2x.png \
    android/assets/pdfjs/web/locale/ach/viewer.properties \
    android/assets/pdfjs/web/locale/af/viewer.properties \
    android/assets/pdfjs/web/locale/ak/viewer.properties \
    android/assets/pdfjs/web/locale/an/viewer.properties \
    android/assets/pdfjs/web/locale/ar/viewer.properties \
    android/assets/pdfjs/web/locale/as/viewer.properties \
    android/assets/pdfjs/web/locale/ast/viewer.properties \
    android/assets/pdfjs/web/locale/az/viewer.properties \
    android/assets/pdfjs/web/locale/be/viewer.properties \
    android/assets/pdfjs/web/locale/bg/viewer.properties \
    android/assets/pdfjs/web/locale/bn-BD/viewer.properties \
    android/assets/pdfjs/web/locale/bn-IN/viewer.properties \
    android/assets/pdfjs/web/locale/br/viewer.properties \
    android/assets/pdfjs/web/locale/brx/viewer.properties \
    android/assets/pdfjs/web/locale/bs/viewer.properties \
    android/assets/pdfjs/web/locale/ca/viewer.properties \
    android/assets/pdfjs/web/locale/cak/viewer.properties \
    android/assets/pdfjs/web/locale/crh/viewer.properties \
    android/assets/pdfjs/web/locale/cs/viewer.properties \
    android/assets/pdfjs/web/locale/csb/viewer.properties \
    android/assets/pdfjs/web/locale/cy/viewer.properties \
    android/assets/pdfjs/web/locale/da/viewer.properties \
    android/assets/pdfjs/web/locale/de/viewer.properties \
    android/assets/pdfjs/web/locale/el/viewer.properties \
    android/assets/pdfjs/web/locale/en-CA/viewer.properties \
    android/assets/pdfjs/web/locale/en-GB/viewer.properties \
    android/assets/pdfjs/web/locale/en-US/viewer.properties \
    android/assets/pdfjs/web/locale/en-ZA/viewer.properties \
    android/assets/pdfjs/web/locale/eo/viewer.properties \
    android/assets/pdfjs/web/locale/es-AR/viewer.properties \
    android/assets/pdfjs/web/locale/es-CL/viewer.properties \
    android/assets/pdfjs/web/locale/es-ES/viewer.properties \
    android/assets/pdfjs/web/locale/es-MX/viewer.properties \
    android/assets/pdfjs/web/locale/et/viewer.properties \
    android/assets/pdfjs/web/locale/eu/viewer.properties \
    android/assets/pdfjs/web/locale/fa/viewer.properties \
    android/assets/pdfjs/web/locale/ff/viewer.properties \
    android/assets/pdfjs/web/locale/fi/viewer.properties \
    android/assets/pdfjs/web/locale/fr/viewer.properties \
    android/assets/pdfjs/web/locale/fy-NL/viewer.properties \
    android/assets/pdfjs/web/locale/ga-IE/viewer.properties \
    android/assets/pdfjs/web/locale/gd/viewer.properties \
    android/assets/pdfjs/web/locale/gl/viewer.properties \
    android/assets/pdfjs/web/locale/gn/viewer.properties \
    android/assets/pdfjs/web/locale/gu-IN/viewer.properties \
    android/assets/pdfjs/web/locale/he/viewer.properties \
    android/assets/pdfjs/web/locale/hi-IN/viewer.properties \
    android/assets/pdfjs/web/locale/hr/viewer.properties \
    android/assets/pdfjs/web/locale/hsb/viewer.properties \
    android/assets/pdfjs/web/locale/hto/viewer.properties \
    android/assets/pdfjs/web/locale/hu/viewer.properties \
    android/assets/pdfjs/web/locale/hy-AM/viewer.properties \
    android/assets/pdfjs/web/locale/ia/viewer.properties \
    android/assets/pdfjs/web/locale/id/viewer.properties \
    android/assets/pdfjs/web/locale/is/viewer.properties \
    android/assets/pdfjs/web/locale/it/viewer.properties \
    android/assets/pdfjs/web/locale/ja/viewer.properties \
    android/assets/pdfjs/web/locale/ka/viewer.properties \
    android/assets/pdfjs/web/locale/kab/viewer.properties \
    android/assets/pdfjs/web/locale/kk/viewer.properties \
    android/assets/pdfjs/web/locale/km/viewer.properties \
    android/assets/pdfjs/web/locale/kn/viewer.properties \
    android/assets/pdfjs/web/locale/ko/viewer.properties \
    android/assets/pdfjs/web/locale/kok/viewer.properties \
    android/assets/pdfjs/web/locale/ks/viewer.properties \
    android/assets/pdfjs/web/locale/ku/viewer.properties \
    android/assets/pdfjs/web/locale/lg/viewer.properties \
    android/assets/pdfjs/web/locale/lij/viewer.properties \
    android/assets/pdfjs/web/locale/lo/viewer.properties \
    android/assets/pdfjs/web/locale/locale.properties \
    android/assets/pdfjs/web/locale/lt/viewer.properties \
    android/assets/pdfjs/web/locale/ltg/viewer.properties \
    android/assets/pdfjs/web/locale/lv/viewer.properties \
    android/assets/pdfjs/web/locale/mai/viewer.properties \
    android/assets/pdfjs/web/locale/meh/viewer.properties \
    android/assets/pdfjs/web/locale/mk/viewer.properties \
    android/assets/pdfjs/web/locale/ml/viewer.properties \
    android/assets/pdfjs/web/locale/mn/viewer.properties \
    android/assets/pdfjs/web/locale/mr/viewer.properties \
    android/assets/pdfjs/web/locale/ms/viewer.properties \
    android/assets/pdfjs/web/locale/my/viewer.properties \
    android/assets/pdfjs/web/locale/nb-NO/viewer.properties \
    android/assets/pdfjs/web/locale/ne-NP/viewer.properties \
    android/assets/pdfjs/web/locale/nl/viewer.properties \
    android/assets/pdfjs/web/locale/nn-NO/viewer.properties \
    android/assets/pdfjs/web/locale/nso/viewer.properties \
    android/assets/pdfjs/web/locale/oc/viewer.properties \
    android/assets/pdfjs/web/locale/or/viewer.properties \
    android/assets/pdfjs/web/locale/pa-IN/viewer.properties \
    android/assets/pdfjs/web/locale/pl/viewer.properties \
    android/assets/pdfjs/web/locale/pt-BR/viewer.properties \
    android/assets/pdfjs/web/locale/pt-PT/viewer.properties \
    android/assets/pdfjs/web/locale/rm/viewer.properties \
    android/assets/pdfjs/web/locale/ro/viewer.properties \
    android/assets/pdfjs/web/locale/ru/viewer.properties \
    android/assets/pdfjs/web/locale/rw/viewer.properties \
    android/assets/pdfjs/web/locale/sah/viewer.properties \
    android/assets/pdfjs/web/locale/sat/viewer.properties \
    android/assets/pdfjs/web/locale/si/viewer.properties \
    android/assets/pdfjs/web/locale/sk/viewer.properties \
    android/assets/pdfjs/web/locale/sl/viewer.properties \
    android/assets/pdfjs/web/locale/son/viewer.properties \
    android/assets/pdfjs/web/locale/sq/viewer.properties \
    android/assets/pdfjs/web/locale/sr/viewer.properties \
    android/assets/pdfjs/web/locale/sv-SE/viewer.properties \
    android/assets/pdfjs/web/locale/sw/viewer.properties \
    android/assets/pdfjs/web/locale/ta-LK/viewer.properties \
    android/assets/pdfjs/web/locale/ta/viewer.properties \
    android/assets/pdfjs/web/locale/te/viewer.properties \
    android/assets/pdfjs/web/locale/th/viewer.properties \
    android/assets/pdfjs/web/locale/tl/viewer.properties \
    android/assets/pdfjs/web/locale/tn/viewer.properties \
    android/assets/pdfjs/web/locale/tr/viewer.properties \
    android/assets/pdfjs/web/locale/tsz/viewer.properties \
    android/assets/pdfjs/web/locale/uk/viewer.properties \
    android/assets/pdfjs/web/locale/ur/viewer.properties \
    android/assets/pdfjs/web/locale/uz/viewer.properties \
    android/assets/pdfjs/web/locale/vi/viewer.properties \
    android/assets/pdfjs/web/locale/wo/viewer.properties \
    android/assets/pdfjs/web/locale/xh/viewer.properties \
    android/assets/pdfjs/web/locale/zam/viewer.properties \
    android/assets/pdfjs/web/locale/zh-CN/viewer.properties \
    android/assets/pdfjs/web/locale/zh-TW/viewer.properties \
    android/assets/pdfjs/web/locale/zu/viewer.properties \
    android/assets/pdfjs/web/viewer.css \
    android/assets/pdfjs/web/viewer.html \
    android/assets/pdfjs/web/viewer.js \
    android/assets/pdfjs/web/viewer.js.map \
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
