#ifndef VIDEOFILTER_H
#define VIDEOFILTER_H

#include <QObject>
#include <QGst/GhostPad>
#include <QGst/Pipeline>

class VideoFilter : public QObject
{
	Q_OBJECT
public:
	explicit VideoFilter(QObject *parent = 0) : QObject(parent) {}
	VideoFilter(QGst::PipelinePtr pipe, QObject *parent)
		: QObject(parent), pipeline(pipe) {}

	void link(VideoFilter *dest);
	void unlink(VideoFilter *dest);

protected:
	QGst::ElementPtr filter;
	QGst::PipelinePtr pipeline;
	QGst::BinPtr bin;
	QGst::ElementPtr videosink;
};

#endif // VIDEOFILTER_H
