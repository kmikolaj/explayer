#include "videobalance.h"
#include <QGst/ElementFactory>

VideoBalance::VideoBalance(QObject *parent) :
    QObject(parent)
{
}

VideoBalance::VideoBalance(QGst::PipelinePtr pipe, QObject *parent) :
	QObject(parent), pipeline(pipe)
{
	init();
}

void VideoBalance::setHue(double)
{

}

void VideoBalance::setSaturation(double)
{

}

void VideoBalance::setBrightness(double)
{

}

void VideoBalance::setContrast(double)
{

}

void VideoBalance::init()
{
	QGst::ElementPtr videosink = QGst::ElementFactory::make("xvimagesink");
	balance = QGst::ElementFactory::make("videobalance");
	bin = QGst::Bin::create("color");
	bin->add(balance);
	QGst::PadPtr pad = balance->getStaticPad("sink");
	QGst::GhostPadPtr ghostpad = QGst::GhostPad::create(pad, "sink");
	bin->addPad(ghostpad);
	bin->add(videosink);
	balance->setProperty("contrast", 2.0);
	balance->link(videosink);
	pipeline->setProperty("video-sink", bin);
}
