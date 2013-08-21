#-------------------------------------------------
#
# Project created by QtCreator 2013-07-14T11:10:31
#
#-------------------------------------------------

QT 		+= core gui widgets x11extras

CONFIG += silent link_pkgconfig
PKGCONFIG += Qt5GStreamer-0.10 Qt5GStreamerUi-0.10 xcb-dpms

QMAKE_CXXFLAGS += -std=c++0x

TARGET = ExPlayer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    player.cpp \
    controls.cpp \
    settings.cpp \
    osd.cpp \
    subtitleeditor.cpp \
    utils.cpp \
    videobalance.cpp \
    videofilter.cpp \
    jumpdialog.cpp \
    metadata.cpp

HEADERS  += mainwindow.h \
    player.h \
    controls.h \
    settings.h \
    osd.h \
    subtitleeditor.h \
    version.h \
    utils.h \
    videobalance.h \
    videofilter.h \
    jumpdialog.h \
    metadata.h

FORMS    += mainwindow.ui \
    controls.ui \
    jumpdialog.ui

OTHER_FILES += \
    README
