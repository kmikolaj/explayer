#ifndef PLAYER_H
#define PLAYER_H

#include <QWidget>
#include <QResizeEvent>
#include <QPaintEvent>
#include "gstreamer.h"

class Player : public QWidget
{
	Q_OBJECT
public:
	explicit Player(QWidget *parent = 0);
	void setVideo(const QString &uri);
	void setSubtitles(const QString &sub, const QString &font, const QString &enc);
	void disableDPMS();
	void enableDPMS();

protected:
	virtual void paintEvent(QPaintEvent *event = 0);

private:
	Gstreamer *output;
	QWidget *video;
	DPMS dpms;

signals:
	void positionChanged();
	void stateChanged();

public slots:
	void play();
	void play(QTime);
	void pause();
	void toggle();
	void stop();
//	void setVolume(double volume);
//	void setHue(double hue);
//	void setSaturation(double saturation);
	void setBrightness(double brightness);
//	void setContrast(double contrast);
	void forceaspectratio();
	void togglesubtitles();
	void toggletime();
	void mute();
};

#endif // PLAYER_H
