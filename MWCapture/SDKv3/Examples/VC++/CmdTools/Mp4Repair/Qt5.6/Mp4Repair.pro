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
DEPENDPATH += $$PWD/../../../../../Include

# destination directory
win32:CONFIG(debug,debug|release){
    TARGET=Mp4RepairQTd
    if(contains(ARCHITECTURE,x86)){
        DESTDIR = $$PWD/../../../../../Bin/Win32/Debug/
        MOC_DIR = $$PWD/../../../../../Temp/Win32/Debug
        LIBS += $$PWD/../../../../../Lib/Win32/LibMWCaptured.lib \
                $$PWD/../../../../../Lib/Win32/mw_mp4_d.lib
    } else {
        DESTDIR = $$PWD/../../../../../Bin/x64/Debug/
        MOC_DIR = $$PWD/../../../../../Temp/x64/Debug
        LIBS += $$PWD/../../../../../Lib/x64/LibMWCaptured.lib \
                $$PWD/../../../../../Lib/x64/mw_mp4_d.lib
    }
}
else:win32:CONFIG(release,debug|release){
    TARGET=Mp4RepairQT
    if(contains(ARCHITECTURE,x86)){
        DESTDIR = $$PWD/../../../../../Bin/Win32/Release/
        MOC_DIR = $$PWD/../../../../../Temp/Win32/Release
        LIBS += $$PWD/../../../../../Lib/Win32/LibMWCapture.lib \
                $$PWD/../../../../../Lib/Win32/mw_mp4.lib
    } else {
        DESTDIR_TARGET = $$PWD/../../../../../Bin/x64/Release/
        MOC_DIR = $$PWD/../../../../../Temp/x64/Release
        LIBS += $$PWD/../../../../../Lib/x64/LibMWCapture.lib \
                $$PWD/../../../../../Lib/x64/mw_mp4.lib
    }
}

TEMPLATE = app

HEADERS += \
    ../src/stdafx.h \
    ../src/targetver.h

SOURCES += \
    ../src/Mp4Repair.cpp \
    ../src/stdafx.cpp

DEFINES  -= UNICODE
