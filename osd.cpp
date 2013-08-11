#include "osd.h"
#include <QGst/ElementFactory>

Osd::Osd(QObject *parent)
	: VideoFilter(parent)
{
}

Osd::Osd(QGst::PipelinePtr pipe, QObject *parent)
    : VideoFilter(pipe, parent), osdVisible(false), timeVisible(false)
{
	init();
}

void Osd::enable()
{
	if (!osdVisible)
	{
		overlay->setProperty("silent", false);
		osdVisible = true;
	}
}

void Osd::disable()
{
	if (osdVisible)
	{
		overlay->setProperty("silent", true);
		osdVisible = false;
	}
}

void Osd::setText(const QString &text, const int time)
{
	if (osdVisible)
	{
		timer.setInterval(time);
		timer.stop();
		timer.start();
		overlay->setProperty("text", text);
	}
}

void Osd::toggleTime()
{
	time->setProperty("silent", timeVisible);
	timeVisible = !timeVisible;
}

void Osd::clear()
{
	if (osdVisible)
	{
		overlay->setProperty("text", QString());
	}
}

void Osd::init()
{
	videosink = QGst::ElementFactory::make("xvimagesink");
	overlay = QGst::ElementFactory::make("textoverlay");
	time = QGst::ElementFactory::make("timeoverlay");
	bin = QGst::Bin::create("osd");

	bin->add(overlay);
	bin->add(time);
	QGst::PadPtr pad = overlay->getStaticPad("video_sink");
	QGst::GhostPadPtr ghostpad = QGst::GhostPad::create(pad, "sink");
	bin->addPad(ghostpad);
	bin->add(videosink);

	overlay->setProperty("text", QString());
	overlay->setProperty("valign", "top");
	overlay->setProperty("halign", "right");
	overlay->setProperty("shaded-background", true);
	overlay->setProperty("font-desc", "Sans Bold 16");

	time->setProperty("halign", "left");
	time->setProperty("silent", !timeVisible);

	overlay->link(time);
	time->link(videosink);
	pipeline->setProperty("video-sink", bin);
	osdVisible = false;

	elements << overlay << time;

	timer.setSingleShot(true);
	connect(&timer, SIGNAL(timeout()), SLOT(clear()));
}
