QT += core
QT -= gui

CONFIG += c++11

TARGET = MergeTool
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    o3dpfile.cpp \
    mergefiles.cpp

HEADERS += \
    o3dpfile.h \
    mergefiles.h

DISTFILES += \
    process.xml
