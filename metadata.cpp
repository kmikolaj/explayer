#include "metadata.h"
#include <QFile>

MetaData::MetaData()
{
	init();
}

bool MetaData::getValid() const
{
	return valid;
}

void MetaData::setValid(bool value)
{
	valid = value;
}

double MetaData::getFramerate() const
{
	return framerate;
}

void MetaData::setFramerate(double value)
{
	framerate = value;
}

UTime MetaData::getDuration() const
{
	return duration;
}

void MetaData::setDuration(const UTime &value)
{
	duration = value;
	frames = value.Frame;
}

qint32 MetaData::getFrames() const
{
	return frames;
}

void MetaData::setFrames(const qint32 &value)
{
	duration = UTime(value);
	frames = value;
}

qint64 MetaData::getSize() const
{
	return size;
}

void MetaData::setSize(const qint64 &value)
{
	size = value;
}

QString MetaData::getFilename() const
{
	return filename;
}

void MetaData::setFilename(const QString &value)
{
	filename = value;
}

QVariantMap MetaData::getTags() const
{
	return tags;
}

void MetaData::setTags(const QVariantMap &value)
{
	tags = value;
}

void MetaData::init()
{
	valid = false;
	framerate = 0.0;
	duration = UTime();
	size = 0;
	filename = "";
}
