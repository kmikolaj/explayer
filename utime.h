#ifndef UTIME_H
#define UTIME_H

#include <QTime>

class UTime
{
public:
	UTime();
	UTime(const QTime &time);
	UTime(const qint32 frame);
	UTime(const qint64 nsec);
	QTime Time;
	qint32 Frame;
	qint64 Nsec;
	void moveNsec(qint64 nsec);
	void moveFrame(qint32 frame);
	static void setFps(double fps);
private:
	static double framerate;
};

#endif // UTIME_H
