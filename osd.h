#ifndef OSD_H
#define OSD_H

#include <QObject>
#include <QTimer>
#include <QGst/Pad>
#include <QGst/Bin>
#include <QGst/GhostPad>
#include <QGst/Pipeline>

class Osd : public QObject
{
	Q_OBJECT
public:
	explicit Osd(QObject *parent = 0);
	Osd(QGst::PipelinePtr, QObject *parent = 0);
	//void toggle();
	void enable();
	void disable();

signals:
	
public slots:
	void setText(const QString &text, const int time=1000);
	void clear();

private:
	void init();
	QGst::PipelinePtr pipeline;
    QGst::ElementPtr overlay;
	QGst::BinPtr osdbin;
	QGst::BinPtr bin;
	QString lasttext;
	bool visible;
	QTimer timer;
};

#endif // OSD_H
