#include "metadata.h"
#include <QFile>
#include <QX11Info>
#include <xcb/dpms.h>

double GstTime::framerate = 0.0;
/*
MetaData::MetaData(const QGst::DiscovererInfoPtr &_info) : info(_info)
{
	if (info->videoStreams().size() > 0)
	{
		videoInfo = info->videoStreams()[0].dynamicCast<QGst::DiscovererVideoInfo>();
		tags = info->tags();
		framerate = double(videoInfo->framerate().numerator) /
		            double(videoInfo->framerate().denominator);
		GstTime::setFps(framerate);
		duration = QGst::ClockTime(info->duration()).toTime();
		frames = duration.Frame;
		filename = info->uri().toLocalFile();
		QFile file(filename);
		size = file.size();
		valid = true;
	}
	else
	{
		init();
	}
}

MetaData::MetaData()
{
	init();
}

void MetaData::init()
{
	valid = false;
	framerate = 0.0;
	duration = GstTime();
	size = 0;
	filename = "";
}
*/
GstTime::GstTime()
{
	Time = QTime(0, 0);
	Nsec = Frame = Msec = 0;
}

GstTime::GstTime(const QTime &time)
{
	Time = time;
	Msec = time.hour() * 3600000 + time.minute() * 60000 + time.second() * 1000 + time.msec();
	Frame = qint32(Msec * GstTime::framerate / 1000.0 + 0.5);
	Nsec = Msec * 1000;
}

GstTime::GstTime(const qint32 frame)
{
	Frame = frame;
	Msec = qint64((frame / GstTime::framerate) * 1000.0);
	Time = QTime(0, 0).addMSecs(Msec);
	Nsec = Msec * 1000;
}

GstTime::GstTime(const qint64 msec)
{
	Msec = msec;
	Frame = qint32(Msec * GstTime::framerate / 1000.0 + 0.5);
	Time = QTime(0, 0).addMSecs(Msec);
	Nsec = Msec * 1000;
}

void GstTime::setFps(double fps)
{
	GstTime::framerate = fps;
}

void GstTime::moveMsec(qint64 msec)
{
	Msec += msec;
	Time = Time.addMSecs(msec);
	Frame = qint32(Msec * GstTime::framerate / 1000.0 + 0.5);
	Nsec += Msec * 1000;
}

void GstTime::moveFrame(qint32 frame)
{
	Frame += frame;
	Msec = qint64((frame / GstTime::framerate) * 1000.0);
	Time = QTime(0, 0).addMSecs(Msec);
	Nsec = Msec * 1000;
}


DPMS::DPMS()
{
	Store();
}

void DPMS::Enable()
{
	xcb_dpms_enable(QX11Info::connection());
}


void DPMS::Disable()
{
	xcb_dpms_disable(QX11Info::connection());
}

void DPMS::Restore()
{
	if (state)
		Enable();
	else
		Disable();
}

void DPMS::Store()
{
	state = State();
}

bool DPMS::State()
{
	u_int8_t state;
	xcb_connection_t *conn = QX11Info::connection();
	xcb_dpms_info_cookie_t dpmsc = xcb_dpms_info(conn);
	xcb_dpms_info_reply_t *dpmsr;
	if ((dpmsr = xcb_dpms_info_reply(conn, dpmsc, NULL)))
	{
		state = dpmsr->state;
		free(dpmsr);
	}
	return bool(state);
}
