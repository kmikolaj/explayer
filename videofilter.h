#ifndef VIDEOFILTER_H
#define VIDEOFILTER_H

#include <QObject>
#include <QVector>
#include <gst/gst.h>

class VideoFilter : public QObject
{
	Q_OBJECT
public:
	explicit VideoFilter(QObject *parent = 0) : QObject(parent) {}
	VideoFilter(GstElement *pipe, QObject *parent)
		: QObject(parent), pipeline(pipe) {}

	void link(VideoFilter *dest);
	void unlink(VideoFilter *dest);
	inline QVector<GstElement*> getElements()
	{
		return elements;
	}

protected:
	QVector<GstElement*> elements;
	GstElement *pipeline;
	GstElement *bin;
	GstElement *videosink;
};

#endif // VIDEOFILTER_H
