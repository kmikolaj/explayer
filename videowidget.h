#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QWidget>
#include <QResizeEvent>
#include <QPaintEvent>
#include <QPainter>
#include "player.h"
#include "settings.h"

class VideoWidget : public QWidget
{
	Q_OBJECT
public:
	explicit VideoWidget(QWidget *parent = 0);
	void createRenderer(const QString &output);
	bool hasRenderer();

	void seek(int seconds);
	void gotoFrame(qint32 frame, bool pause = false);
	void gotoTime(const QTime &time, bool pause = false);
	QString getVideoPath();
	UTime getDuration();

	void setVideo(const QString&);
	void setSubtitles(const QString&);
	void setSubtitlesFont(const QString&, const QString&);

protected:
	void paintEvent(QPaintEvent *event);

private:
	PlayerInterface *player;
	Settings *settings;

signals:
	void videoChanged();
	void positionChanged(UTime);
	void volumeChanged(double);
	void stateChanged(PlayerInterface::State);

private slots:
	void onPositionChanged();

public slots:
	void play(const QTime &time = QTime());
	void pause();
	void toggle();
	void stop();
	void nextFrame();
	void prevFrame();
	void seekForward();
	void seekBackward();
	void seekFrame(qint32);
	void setVolume(double volume);
	void volumeUp();
	void volumeDown();
	void hueUp();
	void hueDown();
	void saturationUp();
	void saturationDown();
	void brightnessUp();
	void brightnessDown();
	void contrastUp();
	void contrastDown();
	void toggleSubtitles();
	void toggleAspectRatio();
	void toggleTime();
	void toggleMute();

};

#endif // VIDEOWIDGET_H
