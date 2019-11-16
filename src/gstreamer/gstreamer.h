#ifndef GSTREAMER_H
#define GSTREAMER_H

#include <QWidget>
#include <QTimer>
#include <QUrl>
#include <QEvent>
#include <gst/gst.h>
#include <gst/video/videooverlay.h>
#include <gst/audio/streamvolume.h>
#include <gst/pbutils/gstdiscoverer.h>
#include "player.h"
#include "utime.h"
#include "osd.h"
#include "balance.h"

class Gstreamer : public PlayerInterface
{
	Q_OBJECT

public:
	Gstreamer(QWidget *window);
	~Gstreamer();
	void setVideo(const QString &path);
	void setSubtitles(const QString &path);
	void setFont(const QString &font, const QString &enc);
	void setPosition(const QTime &pos);
	void setPosition(const qint32 frame);

	UTime position() const;
	UTime duration() const;
	double volume() const;
	double hue() const;
	double saturation() const;
	double brightness() const;
	double contrast() const;

	void expose();
	void setOverlay();
	void setVideoArea(const QRect &rect);
	void setHardwareAcceleration(bool enable);

	bool isSeekable() const;

private:
	GstElement *pipeline;
	GstVideoOverlay *xoverlay;
	GstDiscoverer *discoverer;

	QTimer positionTimer;

	Osd *osd;
	Balance *balance;

	QUrl sub;

	GstState state() const;
	void refresh();

	QUrl makeUrl(const QString &path);

	void seek(const UTime &pos, GstSeekFlags flags = GST_SEEK_FLAG_NONE);
	void setBalanceChannel(const QString &name, double value);
	double getBalanceChannel(const QString &name) const;

	static gboolean busCallback(GstBus *bus, GstMessage *msg, gpointer args);
	static void handlePipelineStateChange(Gstreamer *gst, GstMessage *msg);

signals:

public slots:
	void play();
	void play(QTime);
	void pause();
	void toggle();
	void stop();
	void setVolume(double volume);
	void setHue(double hue);
	void setSaturation(double saturation);
	void setBrightness(double brightness);
	void setContrast(double contrast);
	void forceaspectratio();
	void togglesubtitles();
	void toggletime();
	void mute();
	void on_metadata_update();
};

#endif // GSTREAMER_H
