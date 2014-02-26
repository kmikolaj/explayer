#-------------------------------------------------
#
# Project created by QtCreator 2013-07-14T11:10:31
#
#-------------------------------------------------

QT 		+= core gui widgets x11extras

CONFIG += silent link_pkgconfig
PKGCONFIG += xcb-dpms gstreamer-1.0 gstreamer-audio-1.0 gstreamer-video-1.0

QMAKE_CXXFLAGS += -std=c++0x

TARGET = ExPlayer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    controls.cpp \
    settings.cpp \
    metadata.cpp \
    subtitleeditor.cpp \
    utils.cpp \
    jumpdialog.cpp \
    gstreamer.cpp \
    videofilter.cpp \
    osd.cpp \
    player.cpp \
    balance.cpp

HEADERS  += mainwindow.h \
    controls.h \
    settings.h \
    subtitleeditor.h \
    metadata.h \
    version.h \
    utils.h \
    jumpdialog.h \
    gstreamer.h \
    videofilter.h \
    osd.h \
    player.h \
    balance.h

FORMS    += mainwindow.ui \
    controls.ui \
    jumpdialog.ui

OTHER_FILES += \
    README
