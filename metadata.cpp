#include "metadata.h"
#include <QFile>
/*
MetaData::MetaData(const GstDiscoverer *info)
    : discoverer(info)
{
	if (info->videoStreams().size() > 0)
	{
		videoInfo = info->videoStreams()[0].dynamicCast<QGst::DiscovererVideoInfo>();
		tags = info->tags();
		framerate = double(videoInfo->framerate().numerator) /
		            double(videoInfo->framerate().denominator);
		UTime::setFps(framerate);
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
*/
MetaData::MetaData()
{
	init();
}

void MetaData::init()
{
	valid = false;
	framerate = 0.0;
	duration = UTime();
	size = 0;
	filename = "";
}
