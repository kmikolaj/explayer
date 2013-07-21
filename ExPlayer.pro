#-------------------------------------------------
#
# Project created by QtCreator 2013-07-14T11:10:31
#
#-------------------------------------------------

QT 		+= core gui widgets

CONFIG += silent link_pkgconfig
PKGCONFIG += Qt5GStreamer-0.10 Qt5GStreamerUi-0.10

QMAKE_CXXFLAGS += -std=c++0x -fpermissive

TARGET = ExPlayer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    player.cpp

HEADERS  += mainwindow.h \
    player.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    README
