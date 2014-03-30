#include "utime.h"

double UTime::framerate = 0.0;

UTime::UTime()
{
	Time = QTime(0, 0);
	Nsec = Frame = Msec = 0;
}

UTime::UTime(const QTime &time)
{
	Time = time;
	Msec = time.hour() * 3600000 + time.minute() * 60000 + time.second() * 1000 + time.msec();
	Frame = qint32(Msec * UTime::framerate / 1000.0 + 0.5);
	Nsec = Msec * 1000;
}

UTime::UTime(const qint32 frame)
{
	Frame = frame;
	Msec = qint64((frame / UTime::framerate) * 1000.0);
	Time = QTime(0, 0).addMSecs(Msec);
	Nsec = Msec * 1000;
}

UTime::UTime(const qint64 msec)
{
	Msec = msec;
	Frame = qint32(Msec * UTime::framerate / 1000.0 + 0.5);
	Time = QTime(0, 0).addMSecs(Msec);
	Nsec = Msec * 1000;
}

void UTime::setFps(double fps)
{
	UTime::framerate = fps;
}

void UTime::moveMsec(qint64 msec)
{
	Msec += msec;
	Time = Time.addMSecs(msec);
	Frame = qint32(Msec * UTime::framerate / 1000.0 + 0.5);
	Nsec += Msec * 1000;
}

void UTime::moveFrame(qint32 frame)
{
	Frame += frame;
	Msec = qint64((frame / UTime::framerate) * 1000.0);
	Time = QTime(0, 0).addMSecs(Msec);
	Nsec = Msec * 1000;
}
