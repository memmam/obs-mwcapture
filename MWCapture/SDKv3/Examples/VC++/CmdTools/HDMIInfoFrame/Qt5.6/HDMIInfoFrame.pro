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
    TARGET=HDMIInfoFrameQTd
    if(contains(ARCHITECTURE,x86)){
        DESTDIR_TARGET = $$PWD/../../../../../Bin/Win32/Debug/HDMIInfoFrameQTd
        MOC_DIR = $$PWD/../../../../../Temp/Win32/Debug
        LIBS += $$PWD/../../../../../Lib/Win32/LibMWCaptured.lib
    } else {
        DESTDIR_TARGET = $$PWD/../../../../../Bin/x64/Debug/HDMIInfoFrameQTd
        MOC_DIR = $$PWD/../../../../../Temp/x64/Debug
        LIBS += $$PWD/../../../../../Lib/x64/LibMWCaptured.lib
    }
}
else:win32:CONFIG(release,debug|release){
    TARGET=HDMIInfoFrameQT
    if(contains(ARCHITECTURE,x86)){
        DESTDIR_TARGET = $$PWD/../../../../../Bin/Win32/Release/HDMIInfoFrameQT
        MOC_DIR = $$PWD/../../../../../Temp/Win32/Release
        LIBS += $$PWD/../../../../../Lib/Win32/LibMWCapture.lib
    } else {
        DESTDIR_TARGET = $$PWD/../../../../../Bin/x64/Release/HDMIInfoFrameQT
        MOC_DIR = $$PWD/../../../../../Temp/x64/Release
        LIBS += $$PWD/../../../../../Lib/x64/LibMWCapture.lib
    }
}

TEMPLATE = app

HEADERS += \
    ../src/stdafx.h \
    ../src/targetver.h

SOURCES += \
    ../src/HDMIInfoFrame.cpp \
    ../src/stdafx.cpp

DEFINES += \
    _UNICODE
