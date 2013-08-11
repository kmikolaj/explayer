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
	QByteArray value = balance->property("hue").toByteArray().replace(",", ".");
	return value.toDouble();
}

double VideoBalance::saturation() const
{
	QByteArray value = balance->property("saturation").toByteArray().replace(",", ".");
	return value.toDouble();
}

double VideoBalance::brightness() const
{
	QByteArray value = balance->property("brightness").toByteArray().replace(",", ".");
	return value.toDouble();
}

double VideoBalance::contrast() const
{
	QByteArray value = balance->property("contrast").toByteArray().replace(",", ".");
	return value.toDouble();
}

void VideoBalance::setHue(double value)
{
	if (value >= -1.0 && value <= 1.0)
		balance->setProperty("hue", value);
}

void VideoBalance::setSaturation(double value)
{
	if (value >= -1.0 && value <= 1.0)
		balance->setProperty("saturation", value + 1.0);
}

void VideoBalance::setBrightness(double value)
{
	if (value >= -1.0 && value <= 1.0)
		balance->setProperty("brightness", value);
}

void VideoBalance::setContrast(double value)
{
	if (value >= -1.0 && value <= 1.0)
		balance->setProperty("contrast", value + 1.0);
}

void VideoBalance::init()
{
	videosink = QGst::ElementFactory::make("xvimagesink");
	balance = QGst::ElementFactory::make("videobalance");
	bin = QGst::Bin::create("balance");

	bin->add(balance);
	QGst::PadPtr pad = balance->getStaticPad("sink");
	QGst::GhostPadPtr ghostpad = QGst::GhostPad::create(pad, "sink");
	bin->addPad(ghostpad);
	bin->add(videosink);

	balance->link(videosink);
	pipeline->setProperty("video-sink", bin);
	elements << balance;
}
