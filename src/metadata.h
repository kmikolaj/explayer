#ifndef METADATA_H
#define METADATA_H

#include <QObject>
#include <QMap>
#include <QVector>
#include "utime.h"

class Video
{
public:
	uint Width;
	uint Height;
	double Framerate;
};

class Audio
{
public:
	uint Channels;
	uint SampleRate;
	QString Language;
};

class Subtitle
{
public:
	QString Language;
};

class Metadata : public QObject
{
	Q_OBJECT
public:
	Metadata(QObject *parent = 0);
	~Metadata();

	double getFramerate(int number) const;

	UTime getDuration() const;
	void setDuration(const UTime &value);

	qint32 getFrames() const;
	void setFrames(const qint32 &value);

	qint64 getSize() const;
	void setSize(const qint64 &value);

	QString getFilename() const;
	void setFilename(const QString &value);

	QMap<QString, QString> getTags() const;
	void addTag(const QString &tag, const QString &value);

	void addVideo(const Video &stream);
	void addAudio(const Audio &stream);
	void addSubtitles(const Subtitle &stream);

	bool getSeekable() const;
	void setSeekable(bool value);

private:
	UTime duration;
	qint32 frames;
	qint64 size;
	QString filename;
	QMap<QString, QString> tags;
	QVector<Video> video;
	QVector<Audio> audio;
	QVector<Subtitle> subtitle;
	bool seekable;

signals:
	void updated();
};

#endif // METADATA_H
