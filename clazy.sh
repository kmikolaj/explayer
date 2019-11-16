#!/bin/sh
BIN=/usr/bin/clazy
if [ -x $BIN ]; then
	$BIN -fPIC -std=c++11 \
	-I /usr/include \
	-I /usr/include/qt \
	-I /usr/include/qt/QtCore \
	-I /usr/include/qt/QtGui \
	-I /usr/include/qt/QtNetwork \
	-I /usr/include/qt/QtXml \
	-I /usr/include/qt/QtWidgets \
	-I /usr/include/qt/QtX11Extras \
	-I /usr/include/gstreamer-1.0 \
	-I /usr/lib/x86_64-linux-gnu/gstreamer-1.0/include \
	-I /usr/include/glib-2.0 \
	-I /usr/lib/x86_64-linux-gnu/glib-2.0/include \
	-I ../build-ExPlayer-Desktop-Debug \
	-I . \
	-c $*
else
	echo "Please install $BIN [https://github.com/KDE/clazy]"
fi
