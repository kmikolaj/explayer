#ifndef VIDEOBALANCE_H
#define VIDEOBALANCE_H

#include "videofilter.h"
#include <QGst/GhostPad>
#include <QGst/Pipeline>

class VideoBalance : public VideoFilter
{
	Q_OBJECT
public:
	explicit VideoBalance(QObject *parent = 0);
	VideoBalance(QGst::PipelinePtr, QObject *parent = 0);

	double hue() const;
	double saturation() const;
	double brightness() const;
	double contrast() const;

signals:

public slots:
	void setHue(double);
	void setSaturation(double);
	void setBrightness(double);
	void setContrast(double);

private:
	void init();
	QGst::ElementPtr balance;
};

#endif // VIDEOBALANCE_H
