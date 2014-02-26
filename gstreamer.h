#ifndef GSTREAMER_H
#define GSTREAMER_H

#include <QWidget>
#include <QTimer>
#include <QUrl>
#include <gst/gst.h>
#include <gst/video/videooverlay.h>
#include <gst/audio/streamvolume.h>
#include "metadata.h"
#include "osd.h"
#include "balance.h"

class Gstreamer : public QObject
{
	Q_OBJECT

public:
	enum SeekFlag { None, Accurate, Skip };

	Gstreamer(QWidget *window);
	~Gstreamer();
	void setVideo(const QString &path);
	void setSubtitles(const QString &path);
	bool embed(QWidget *window);

	void seek(const UTime &pos);
	UTime position() const;
	UTime duration() const;
	double volume() const;
	double hue() const;
	double saturation() const;
	double brightness() const;
	double contrast() const;

	void setPosition(const UTime &pos, SeekFlag flag = None);

	void expose();
	void setVideoArea(const QRect &rect);
	void setHardwareAcceleration(bool enable);

//	GstTime length() const;
//	MetaData metadata() const;

private:
	QWidget *surface;
	GstElement *pipeline;
	GstVideoOverlay *xoverlay;

	QTimer positionTimer;

	Osd *osd;
	Balance *balance;

	QString video;
	QString sub;

	GstState state();
	void refresh();

	QUrl makeUrl(const QString &path);

	static gboolean busCallback(GstBus *bus, GstMessage *msg, gpointer args);
	static void handlePipelineStateChange(Gstreamer *gst, GstMessage *msg);

signals:
	void positionChanged();
	void stateChanged();

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
};

#endif // GSTREAMER_H
