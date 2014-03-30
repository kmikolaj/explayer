#ifndef GSTREAMER_H
#define GSTREAMER_H

#include <QWidget>
#include <QTimer>
#include <QUrl>
#include <gst/gst.h>
#if GST_VERSION_MAJOR == 1
#include <gst/video/videooverlay.h>
#include <gst/audio/streamvolume.h>
#else
#include <gst/interfaces/xoverlay.h>
#include <gst/interfaces/streamvolume.h>
#endif
#include "player.h"
#include "utime.h"
#include "osd.h"
#include "balance.h"
#include "metadata.h"

class Gstreamer : public PlayerInterface
{
	Q_OBJECT
public:
	enum SeekFlag { None=0, Accurate=2, Skip=16 };

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
	void setVideoArea(const QRect &rect);
	void setHardwareAcceleration(bool enable);

	bool canSeek();
//	MetaData metadata() const;

protected:
//	void paintEvent(QPaintEvent *event = 0);

private:
	QWidget *surface;
	GstElement *pipeline;
#if GST_VERSION_MAJOR == 1
	GstVideoOverlay *xoverlay;
#else
	GstXOverlay *xoverlay;
#endif

	QTimer positionTimer;

	Osd *osd;
	Balance *balance;

	QString video;
	QString sub;

	GstState state();
	void refresh();

	QUrl makeUrl(const QString &path);

	void seek(const UTime &pos, SeekFlag flag = None);

	static gboolean busCallback(GstBus *bus, GstMessage *msg, gpointer args);
	static void handlePipelineStateChange(Gstreamer *gst, GstMessage *msg);

signals:
	void positionChanged();
	void stateChanged(); // wrzuć do playerI ?

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
