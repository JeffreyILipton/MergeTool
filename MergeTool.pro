QT += core
QT -= gui

CONFIG += c++11

TARGET = MergeTool
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    o3dpfile.cpp \
    mergefiles.cpp \
    o3dpstreamer.cpp

HEADERS += \
    o3dpfile.h \
    mergefiles.h \
    o3dpstreamer.h

DISTFILES += \
    process.xml
