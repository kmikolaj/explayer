#ifndef PLAYER_H
#define PLAYER_H

#include <QWidget>
#include "dpms.h"
#include "utime.h"
#include "metadata.h"

class PlayerInterface : public QObject
{
	Q_OBJECT
public:
	explicit PlayerInterface(QWidget *parent);
	~PlayerInterface();
	virtual void setVideo(const QString &uri) = 0;
	virtual void setSubtitles(const QString &sub) = 0;
	virtual void setFont(const QString &font, const QString &enc) = 0;
	virtual void setPosition(const QTime &pos) = 0;
	virtual void setPosition(const qint32 frame) = 0;

	virtual bool canSeek() = 0;
	virtual UTime position() const = 0;
	virtual UTime duration() const = 0;
	virtual double volume() const = 0;
	virtual double hue() const = 0;
	virtual double saturation() const = 0;
	virtual double brightness() const = 0;
	virtual double contrast() const = 0;

	virtual void expose() = 0;

	void disableDPMS();
	void enableDPMS();

	enum State
	{
		UNKNOWN,
		PLAYING,
		PAUSED,
		STOPPED,
	};

	Metadata *getMetadata() const;
	void setMetadata(Metadata *value);
	QString getVideoPath() const;

protected:
	QWidget *surface;
	Metadata *metadata;
	WId xwinid;
	QString videoPath;

private:
	DPMS dpms;

signals:
	void positionChanged();
	void volumeChanged(double);
	void stateChanged(PlayerInterface::State);

public slots:
	virtual void play() = 0;
	virtual void play(QTime) = 0;
	virtual void pause() = 0;
	virtual void toggle() = 0;
	virtual void stop() = 0;
	virtual void setVolume(double volume) = 0;
	virtual void setHue(double hue) = 0;
	virtual void setSaturation(double saturation) = 0;
	virtual void setBrightness(double brightness) = 0;
	virtual void setContrast(double contrast) = 0;
	virtual void forceaspectratio() = 0;
	virtual void togglesubtitles() = 0;
	virtual void toggletime() = 0;
	virtual void mute() = 0;
};

#endif // PLAYER_H
