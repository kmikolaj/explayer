#ifndef METADATA_H
#define METADATA_H

#include <gst/pbutils/gstdiscoverer.h>
#include <QVariantMap>
#include "utime.h"

class MetaData
{
public:
	MetaData();
//	MetaData(const GstDiscovererInfo *info);
	inline double getFramerate() const
	{
		return framerate;
	}
	inline UTime getDuration() const
	{
		return duration;
	}
	inline quint64 getSize() const
	{
		return size;
	}
	inline QString getFileName() const
	{
		return filename;
	}
	inline bool isValid() const
	{
		return valid;
	}
	inline QVariantMap getTags() const
	{
		return tags;
	}
private:
//	GstDiscoverer *discoverer;
//	GstDiscovererInfo *videoInfo;
	bool valid;
	double framerate;
	UTime duration;
	quint32 frames;
	quint64 size;
	QString filename;
	QVariantMap tags;

	void init();
};

#endif // METADATA_H
