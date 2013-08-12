#ifndef PLAYER_H
#define PLAYER_H

#include <QTimer>
#include <QTime>
#include <QGst/Pipeline>
#include <QGst/Ui/VideoWidget>
#include <QGst/Discoverer>
#include <QUrl>
#include "settings.h"
#include "osd.h"
#include "videobalance.h"

enum SeekFlag
{
    None,
    Accurate,
    Skip
};

enum ColorBalance
{
    Contrast,
    Brightness,
    Hue,
    Saturation
};

class GstTime
{
public:
	GstTime();
	GstTime(const QTime &time);
	GstTime(const qint32 frame);
	GstTime(const qint64 msec);
	static void setFps(double fps);
	QTime Time;
	qint32 Frame;
	qint64 Msec;
	qint64 Nsec;
	bool Valid()
	{
		return (framerate > 0.0);
	}
	void moveMsec(qint64 msec);
	void moveFrame(qint32 frame);
private:
	static double framerate;
};

class MetaData
{
public:
	MetaData();
	MetaData(const QGst::DiscovererInfoPtr &_info);
	inline double getFramerate() const
	{
		return framerate;
	}
	inline GstTime getDuration() const
	{
		return duration;
	}
	inline quint64 getSize() const
	{
		return size;
	}
	inline QString getFileName() const
	{
		return filename;
	}
	inline bool isValid() const
	{
		return valid;
	}
	inline QGst::TagList getTags() const
	{
		return tags;
	}
private:
	QGst::DiscovererInfoPtr info;
	QGst::DiscovererVideoInfoPtr videoInfo;
	bool valid;
	double framerate;
	GstTime duration;
	quint32 frames;
	quint64 size;
	QString filename;
	QGst::TagList tags;

	void init();
};

class Player : public QGst::Ui::VideoWidget
{
	Q_OBJECT

public:
	Player(QWidget *parent = 0);
	~Player();

	void setUri(const QString &uri);
	void setSubtitles(const QString &sub, const QString &font, const QString &enc);

	GstTime position() const;
	void setPosition(const GstTime &pos, SeekFlag flag = None);
	double volume() const;
	double hue() const;
	double saturation() const;
	double brightness() const;
	double contrast() const;

	QGst::State state() const;
	GstTime length() const;
	MetaData metadata() const;

public slots:
	void play();
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

signals:
	void positionChanged();
	void stateChanged();

private:
	void onBusMessage(const QGst::MessagePtr &message);
	void handlePipelineStateChange(const QGst::StateChangedMessagePtr &scm);
	void extractMetaData();
	void setHardwareAcceleration(bool enable);

	QGst::PipelinePtr pipeline;
	QString videouri;
	QString suburi;
	bool subtitles;
	bool aspectratio;
	MetaData meta;
	QTimer positionTimer;
	Settings *settings;
	Osd *osd;
	VideoBalance *balance;
};

#endif
