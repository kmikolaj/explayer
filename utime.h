#ifndef UTIME_H
#define UTIME_H

#include <QTime>

class UTime
{
public:
	UTime();
	UTime(const QTime &time);
	UTime(const qint32 frame);
	UTime(const qint64 msec);
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

#endif // UTIME_H
