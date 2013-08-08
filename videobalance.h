#ifndef VIDEOBALANCE_H
#define VIDEOBALANCE_H

#include <QObject>
#include <QGst/GhostPad>
#include <QGst/Pipeline>

class VideoBalance : public QObject
{
	Q_OBJECT
public:
	explicit VideoBalance(QObject *parent = 0);
	VideoBalance(QGst::PipelinePtr, QObject *parent = 0);

	int hue() const;
	int saturation() const;
	int brightness() const;
	int contrast() const;
	QGst::BinPtr element() { return bin; }
	
signals:
	
public slots:
	void setHue(double);
	void setSaturation(double);
	void setBrightness(double);
	void setContrast(double);

private:
	void init();
	QGst::PipelinePtr pipeline;
	QGst::ElementPtr balance;
	QGst::BinPtr bin;
};

#endif // VIDEOBALANCE_H
