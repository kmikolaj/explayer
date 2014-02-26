#include "osd.h"

Osd::Osd(QObject *parent)
	: VideoFilter(parent)
{
}

Osd::Osd(GstElement *pipeline, QObject *parent)
	: VideoFilter(pipeline, parent), osdVisible(false), timeVisible(false)
{
	init();
}

void Osd::enable()
{
	if (!osdVisible)
	{
		g_object_set(G_OBJECT(overlay), "silent", false, NULL);
		osdVisible = true;
	}
}

void Osd::disable()
{
	if (osdVisible)
	{
		g_object_set(G_OBJECT(overlay), "silent", true, NULL);
		osdVisible = false;
	}
}

void Osd::setText(const QString &text, const int time)
{
	if (osdVisible)
	{
		/*
		timer.setInterval(time);
		timer.stop();
		timer.start();
		g_object_set(G_OBJECT(overlay), "text", text.toUtf8().data(), NULL);
		*/
	}
}

void Osd::toggleTime()
{
	g_object_set(G_OBJECT(time), "silent", timeVisible, NULL);
	timeVisible = !timeVisible;
}

void Osd::clear()
{
	if (osdVisible)
	{
		g_object_set(G_OBJECT(overlay), "text", "", NULL);
	}
}

void Osd::init()
{
	g_object_get(G_OBJECT(pipeline), "video-sink", &videosink, NULL);
	overlay = gst_element_factory_make("textoverlay", "overlay1");
//	time = gst_element_factory_make("timeoverlay", "time1");
	bin = gst_bin_new("osd");

	int r = gst_bin_add(GST_BIN(bin), overlay);
//	r = gst_bin_add(GST_BIN(bin), time);
	GstPad *pad = gst_element_get_static_pad(overlay, "video_sink");
	r = gst_element_add_pad(bin, gst_ghost_pad_new("sink", pad));
	gst_object_unref(GST_OBJECT(pad));
	qDebug() << r;
//	r = gst_bin_remove(GST_BIN(pipeline), videosink);
	qDebug() << r;
//	r = gst_bin_add(GST_BIN(bin), videosink);
	qDebug() << r;
	g_object_set(G_OBJECT(overlay), "text", "AAAAAAAAAAAA", NULL);
//	g_object_set(G_OBJECT(overlay), "valign", "top", NULL);
//	g_object_set(G_OBJECT(overlay), "halign", "right", NULL);
//	g_object_set(G_OBJECT(overlay), "shaded-background", true, NULL);
//	g_object_set(G_OBJECT(overlay), "font-desc", "Sans Bold 16", NULL);

//	g_object_set(time, "halign", "left", NULL);
//	g_object_set(G_OBJECT(time), "silent", !timeVisible, NULL);

//	r = gst_element_link(overlay, time);
//	r = gst_element_link(time, videosink);
gst_element_link_pads(overlay, "src", videosink, "sink");
	//r = gst_element_link_pads_filtered(overlay, NULL, videosink, NULL, NULL);
		//r = gst_element_link(overlay, videosink);
//		r = gst_element_link(videosink, overlay);

	g_object_set(G_OBJECT(pipeline), "video-sink", bin, NULL);

	osdVisible = false;

	elements << overlay;// << time;

	timer.setSingleShot(true);
	connect(&timer, SIGNAL(timeout()), SLOT(clear()));
}
