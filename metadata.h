#ifndef METADATA_H
#define METADATA_H

#include <QVariantMap>
#include "utime.h"

class MetaData
{
public:
	MetaData();

	bool getValid() const;
	void setValid(bool value);

	double getFramerate() const;
	void setFramerate(double value);

	UTime getDuration() const;
	void setDuration(const UTime &value);

	qint32 getFrames() const;
	void setFrames(const qint32 &value);

	qint64 getSize() const;
	void setSize(const qint64 &value);

	QString getFilename() const;
	void setFilename(const QString &value);

	QVariantMap getTags() const;
	void setTags(const QVariantMap &value);

private:
	bool valid;
	double framerate;
	UTime duration;
	qint32 frames;
	qint64 size;
	QString filename;
	QVariantMap tags;
	bool seekable;

	void init();
};

#endif // METADATA_H
