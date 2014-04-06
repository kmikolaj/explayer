#include "metadata.h"
#include <QFile>

Metadata::Metadata(QObject *parent)
	: QObject(parent)
{
	duration = UTime();
	size = 0;
	filename = "";
}

Metadata::~Metadata()
{

}

double Metadata::getFramerate(int number) const
{
	if (video.size() >= number && number > 0)
		return video[number - 1].Framerate;
	return 1.0;
}

UTime Metadata::getDuration() const
{
	return duration;
}

void Metadata::setDuration(const UTime &value)
{
	duration = value;
	frames = value.Frame;
}

qint32 Metadata::getFrames() const
{
	return frames;
}

void Metadata::setFrames(const qint32 &value)
{
	duration = UTime(value);
	frames = value;
}

qint64 Metadata::getSize() const
{
	return size;
}

void Metadata::setSize(const qint64 &value)
{
	size = value;
}

QString Metadata::getFilename() const
{
	return filename;
}

void Metadata::setFilename(const QString &value)
{
	filename = value;
}

QMap<QString, QString> Metadata::getTags() const
{
	return tags;
}

void Metadata::addTag(const QString &tag, const QString &value)
{
	tags[tag] = value;
}

void Metadata::addVideo(const Video &stream)
{
	video.append(stream);
}

void Metadata::addAudio(const Audio &stream)
{
	audio.append(stream);
}

void Metadata::addSubtitles(const Subtitle &stream)
{
	subtitle.append(stream);
}

bool Metadata::getSeekable() const
{
	return seekable;
}

void Metadata::setSeekable(bool value)
{
	seekable = value;
}

