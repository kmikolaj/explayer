#ifndef OSD_H
#define OSD_H

#include "videofilter.h"
#include <QTimer>
#include <gst/gst.h>

class Osd : public VideoFilter
{
	Q_OBJECT
public:
	explicit Osd(QObject *parent = 0);
	Osd(GstElement *pipeline, QObject *parent = 0);

	void enable();
	void disable();

signals:

public slots:
	void setText(const QString &text, const int time = 1000);
	void toggleTime();
	void clear();

private:
	void init();
	bool osdVisible;
	bool timeVisible;
	GstElement *overlay;
	GstElement *time;
	QTimer timer;
};

#endif // OSD_H
