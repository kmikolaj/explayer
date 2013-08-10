#include "osd.h"
#include <QGst/ElementFactory>

Osd::Osd(QObject *parent)
	: VideoFilter(parent)
{
}

Osd::Osd(QGst::PipelinePtr pipe, QObject *parent)
	: VideoFilter(pipe, parent), lasttext(QString()), visible(false)
{
	init();
}

void Osd::enable()
{
	if (!visible)
	{
		filter->setProperty("text", lasttext);
		visible = true;
	}
}

void Osd::disable()
{
	if (visible)
	{
		filter->setProperty("text", QString());
		visible = false;
	}
}

void Osd::setText(const QString &text, const int time)
{
	if (visible)
	{
		timer.setInterval(time);
		timer.stop();
		timer.start();
		filter->setProperty("text", lasttext = text);
	}
}

void Osd::showTime()
{
	// TODO
}

void Osd::clear()
{
	if (visible)
	{
		filter->setProperty("text", QString());
	}
}

void Osd::init()
{
	videosink = QGst::ElementFactory::make("xvimagesink");
	filter = QGst::ElementFactory::make("textoverlay");
	bin = QGst::Bin::create("osd");

	bin->add(filter);
	QGst::PadPtr pad = filter->getStaticPad("video_sink");
	QGst::GhostPadPtr ghostpad = QGst::GhostPad::create(pad, "sink");
	bin->addPad(ghostpad);
	bin->add(videosink);

	filter->setProperty("text", lasttext);
	filter->setProperty("valign", "top");
	filter->setProperty("halign", "left");
	filter->setProperty("shaded-background", true);
	filter->setProperty("font-desc", "Sans Bold 16");

	filter->link(videosink);
	pipeline->setProperty("video-sink", bin);
	visible = false;

	timer.setSingleShot(true);
	connect(&timer, SIGNAL(timeout()), SLOT(clear()));
}
