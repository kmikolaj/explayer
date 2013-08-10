#include "videobalance.h"
#include <QGst/ElementFactory>

VideoBalance::VideoBalance(QObject *parent)
	: VideoFilter(parent)
{
}

VideoBalance::VideoBalance(QGst::PipelinePtr pipe, QObject *parent) :
	VideoFilter(pipe, parent)
{
	init();
}

double VideoBalance::hue() const
{
	return filter->property("hue").toByteArray().toDouble();
}

double VideoBalance::saturation() const
{
	return filter->property("saturation").toByteArray().toDouble();
}

double VideoBalance::brightness() const
{
	return filter->property("brightness").toByteArray().toDouble();
}

double VideoBalance::contrast() const
{
	return filter->property("contrast").toByteArray().toDouble();
}

void VideoBalance::setHue(double value)
{
	if (value >= -1.0 && value <= 1.0)
		filter->setProperty("hue", value);
}

void VideoBalance::setSaturation(double value)
{
	if (value >= -1.0 && value <= 1.0)
		filter->setProperty("saturation", value + 1.0);
}

void VideoBalance::setBrightness(double value)
{
	if (value >= -1.0 && value <= 1.0)
		filter->setProperty("brightness", value);
}

void VideoBalance::setContrast(double value)
{
	if (value >= -1.0 && value <= 1.0)
		filter->setProperty("contrast", value + 1.0);
}

void VideoBalance::init()
{
	videosink = QGst::ElementFactory::make("xvimagesink");
	filter = QGst::ElementFactory::make("videobalance");
	bin = QGst::Bin::create("balance");

	bin->add(filter);
	QGst::PadPtr pad = filter->getStaticPad("sink");
	QGst::GhostPadPtr ghostpad = QGst::GhostPad::create(pad, "sink");
	bin->addPad(ghostpad);
	bin->add(videosink);

	filter->link(videosink);
	pipeline->setProperty("video-sink", bin);
}
