#ifndef METADATA_H
#define METADATA_H

//#include <QGst/Discoverer>
#include <QTime>

class DPMS
{
public:
	DPMS();
	void Enable();
	void Disable();
	void Restore();
private:
	void Store();
	bool State();
	bool state;
};

class GstTime
{
public:
	GstTime();
	GstTime(const QTime &time);
	GstTime(const qint32 frame);
	GstTime(const qint64 msec);
	static void setFps(double fps);
	QTime Time;
	qint32 Frame;
	qint64 Msec;
	qint64 Nsec;
	bool Valid()
	{
		return (framerate > 0.0);
	}
	void moveMsec(qint64 msec);
	void moveFrame(qint32 frame);
private:
	static double framerate;
};
/*
class MetaData
{
public:
	MetaData();
	MetaData(const QGst::DiscovererInfoPtr &_info);
	inline double getFramerate() const
	{
		return framerate;
	}
	inline GstTime getDuration() const
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
	inline QGst::TagList getTags() const
	{
		return tags;
	}
private:
	QGst::DiscovererInfoPtr info;
	QGst::DiscovererVideoInfoPtr videoInfo;
	bool valid;
	double framerate;
	GstTime duration;
	quint32 frames;
	quint64 size;
	QString filename;
	QGst::TagList tags;

	void init();
};
*/
#endif // METADATA_H
