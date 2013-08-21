#ifndef PLAYER_H
#define PLAYER_H

#include <QTimer>
#include <QGst/Pipeline>
#include <QGst/Ui/VideoWidget>
#include "settings.h"
#include "osd.h"
#include "videobalance.h"
#include "metadata.h"

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
	void disableDPMS();
	void enableDPMS();

	QGst::PipelinePtr pipeline;
	QString videouri;
	QString suburi;
	bool subtitles;
	bool aspectratio;
	MetaData meta;
	QTimer positionTimer;
	DPMS dpms;
	Settings *settings;
	Osd *osd;
	VideoBalance *balance;
};

#endif
