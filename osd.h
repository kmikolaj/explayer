#ifndef OSD_H
#define OSD_H

#include "videofilter.h"
#include <QTimer>
#include <QGst/GhostPad>
#include <QGst/Pipeline>

class Osd : public VideoFilter
{
	Q_OBJECT
public:
	explicit Osd(QObject *parent = 0);
	Osd(QGst::PipelinePtr, QObject *parent = 0);

	void enable();
	void disable();

signals:

public slots:
	void setText(const QString &text, const int time = 1000);
	void showTime();
	void clear();

private:
	void init();
	QString lasttext;
	bool visible;
	QTimer timer;
};

#endif // OSD_H
