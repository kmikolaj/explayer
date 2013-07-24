#-------------------------------------------------
#
# Project created by QtCreator 2013-07-14T11:10:31
#
#-------------------------------------------------

QT 		+= core gui widgets

CONFIG += silent link_pkgconfig
PKGCONFIG += Qt5GStreamer-0.10 Qt5GStreamerUi-0.10

QMAKE_CXXFLAGS += -std=c++0x

TARGET = ExPlayer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    player.cpp \
    controls.cpp \
    settings.cpp

HEADERS  += mainwindow.h \
    player.h \
    controls.h \
    settings.h

FORMS    += mainwindow.ui \
    controls.ui

OTHER_FILES += \
    README
