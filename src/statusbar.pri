CONFIG += c++11
INCLUDEPATH += $$PWD

HEADERS +=

SOURCES += \
    $$PWD/statusbar.cpp

android {
    QT += androidextras

    SOURCES += \
        $$PWD/statusbar_android.cpp
} else:ios {
    LIBS += -framework UIKit

    OBJECTIVE_SOURCES += \
        $$PWD/statusbar_ios.mm
} else {
    SOURCES += \
        $$PWD/statusbar_dummy.cpp
}
