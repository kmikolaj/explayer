#include "utime.h"
#include <gst/gst.h>

double UTime::framerate = 0.0;

UTime::UTime()
{
	Time = QTime(0, 0);
	Nsec = Frame = 0;
}

UTime::UTime(const QTime &time)
{
	Time = time;
	Nsec = (time.hour() * 3600000 + time.minute() * 60000 + time.second() * 1000 + time.msec()) * GST_MSECOND;
	Frame = qint32(Nsec / double(GST_SECOND) * UTime::framerate  + 0.5);
}

UTime::UTime(const qint32 frame)
{
	Frame = frame;
	Nsec = qint64((frame / UTime::framerate) * double(GST_SECOND) + 0.5);
	Time = QTime(0, 0).addMSecs(Nsec / GST_MSECOND);
}

UTime::UTime(const qint64 nsec)
{
	Nsec = nsec;
	Frame = qint32(Nsec / double(GST_SECOND) * UTime::framerate  + 0.5);
	Time = QTime(0, 0).addMSecs(Nsec / GST_MSECOND);
}

void UTime::setFps(double fps)
{
	UTime::framerate = fps;
}

void UTime::moveNsec(qint64 nsec)
{
	Nsec = MAX(Nsec + nsec, 0);
	Time = QTime(0, 0).addMSecs(Nsec / GST_MSECOND);
	Frame = qint32(Nsec / double(GST_SECOND) * UTime::framerate  + 0.5);
}

void UTime::moveFrame(qint32 frame)
{
	Frame = MAX(Frame + frame, 0);
	Nsec = qint64((Frame / UTime::framerate) * double(GST_SECOND) + 0.5);
	Time = QTime(0, 0).addMSecs(Nsec / GST_MSECOND);
}
