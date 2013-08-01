#include "osd.h"
#include <QGst/ElementFactory>

Osd::Osd(QObject *parent) :
	QObject(parent)
{
}

Osd::Osd(QGst::PipelinePtr pipe, QObject *parent)
	: QObject(parent), pipeline(pipe), lasttext(QString()), visible(false)
{
	init();
}

void Osd::enable()
{
	if (!visible)
	{
		overlay->setProperty("text", lasttext);
		visible = true;
	}
}

void Osd::disable()
{
	if (visible)
	{
		overlay->setProperty("text", QString());
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
		overlay->setProperty("text", lasttext = text);
	}
}

void Osd::clear()
{
	if (visible)
	{
		overlay->setProperty("text", QString());
	}
}

void Osd::init()
{
	QGst::ElementPtr videosink = QGst::ElementFactory::make("xvimagesink");
	overlay = QGst::ElementFactory::make("textoverlay");
	osdbin = QGst::Bin::create("osd");

	osdbin->add(overlay);
	QGst::PadPtr pad = overlay->getStaticPad("video_sink");
	QGst::GhostPadPtr ghostpad = QGst::GhostPad::create(pad, "sink");
	osdbin->addPad(ghostpad);
	osdbin->add(videosink);

	overlay->setProperty("text", lasttext);
	overlay->setProperty("valign", "top");
	overlay->setProperty("halign", "left");
	overlay->setProperty("shaded-background", true);
	overlay->setProperty("font-desc", "Sans Bold 16");

	overlay->link(videosink);

	pipeline->setProperty("video-sink", osdbin);
	visible = false;

	timer.setSingleShot(true);
	connect(&timer, SIGNAL(timeout()), SLOT(clear()));
}
