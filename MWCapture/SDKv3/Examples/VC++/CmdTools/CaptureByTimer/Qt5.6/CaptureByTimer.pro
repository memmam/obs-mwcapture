# get target architecture
greaterThan(QT_MAJOR_VERSION,4){
    TARGET_ARCH=$${QT_ARCH}
} else {
    TARGET_ARCH=$${QMAKE_HOST.arch}
}
contains(TARGET_ARCH,x86_64) {
    ARCHITECTURE =x64
} else {
    ARCHITECTURE =x86
}

QT += core
QT -= gui

CONFIG += c++11

CONFIG += console
CONFIG -= app_bundle

# additional include path
INCLUDEPATH += $$PWD/../../../../../Include
INCLUDEPATH += $$PWD/../../Common
DEPENDPATH += $$PWD/../../../../../Include


# destination directory
win32:CONFIG(debug,debug|release){
    TARGET=CaptureByTimerQTd
    if(contains(ARCHITECTURE,x86)){
        DESTDIR_TARGET = $$PWD/../../../../../Bin/Win32/Debug/CaptureByTimerQTd
        MOC_DIR = $$PWD/../../../../../Temp/Win32/Debug
        LIBS += $$PWD/../../../../../Lib/Win32/LibMWCaptured.lib
        LIBS += $$PWD/../../../../../../3rdPart/GDIPlus/lib/Win32/GdiPlus.lib
    } else {
        DESTDIR_TARGET = $$PWD/../../../../../Bin/x64/Debug/CaptureByTimerQTd
        MOC_DIR = $$PWD/../../../../../Temp/x64/Debug
        LIBS += $$PWD/../../../../../Lib/x64/LibMWCaptured.lib
        LIBS += $$PWD/../../../../../../3rdPart/GDIPlus/lib/x64/GdiPlus.lib
    }
}
else:win32:CONFIG(release,debug|release){
    TARGET=CaptureByTimerQT
    if(contains(ARCHITECTURE,x86)){
        DESTDIR_TARGET = $$PWD/../../../../../Bin/Win32/Release/CaptureByTimerQT
        MOC_DIR = $$PWD/../../../../../Temp/Win32/Release
        LIBS += $$PWD/../../../../../Lib/Win32/LibMWCapture.lib
        LIBS += $$PWD/../../../../../../3rdPart/GDIPlus/lib/Win32/GdiPlus.lib
    } else {
        DESTDIR_TARGET = $$PWD/../../../../../Bin/x64/Release/CaptureByTimerQT
        MOC_DIR = $$PWD/../../../../../Temp/x64/Release
        LIBS += $$PWD/../../../../../Lib/x64/LibMWCapture.lib
        LIBS += $$PWD/../../../../../../3rdPart/GDIPlus/lib/x64/GdiPlus.lib
    }
}


TEMPLATE = app

HEADERS += \
    ../src/stdafx.h \
    ../src/targetver.h

SOURCES += \
    ../src/CaptureByTimer.cpp \
    ../src/stdafx.cpp

DEFINES += \
    _UNICODE
