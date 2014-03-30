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
		g_object_set(G_OBJECT(overlay), "silent", false, nullptr);
		osdVisible = true;
	}
}

void Osd::disable()
{
	if (osdVisible)
	{
		g_object_set(G_OBJECT(overlay), "silent", true, nullptr);
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
		g_object_set(G_OBJECT(overlay), "text", text.toUtf8().data(), nullptr);
	}
}

void Osd::toggleTime()
{
	g_object_set(G_OBJECT(time), "silent", timeVisible, nullptr);
	timeVisible = !timeVisible;
}

void Osd::clear()
{
	if (osdVisible)
	{
		g_object_set(G_OBJECT(overlay), "text", "", nullptr);
	}
}

void Osd::init()
{
	videosink = gst_element_factory_make("autovideosink", "video1");
	overlay = gst_element_factory_make("textoverlay", "overlay1");
	time = gst_element_factory_make("timeoverlay", "time1");
	bin = gst_bin_new("osd");

  	gst_bin_add_many (GST_BIN (bin), videosink, time, overlay, nullptr);
	gst_element_link_many(overlay, time, videosink, nullptr);
	GstPad *pad = gst_element_get_static_pad(overlay, "video_sink");
	GstPad *ghostpad = gst_ghost_pad_new("sink", pad);
	gst_element_add_pad(bin, ghostpad);
	gst_object_unref(GST_OBJECT(pad));

	g_object_set(G_OBJECT(overlay), "text", "", nullptr);
	g_object_set(G_OBJECT(overlay), "silent", false, nullptr);
#if GST_VERSION_MAJOR == 1
	g_object_set(G_OBJECT(overlay), "valignment", "top", nullptr);
	g_object_set(G_OBJECT(overlay), "halignment", "right", nullptr);
#else
	g_object_set(G_OBJECT(overlay), "valign", "top", nullptr);
	g_object_set(G_OBJECT(overlay), "halign", "right", nullptr);
#endif
	g_object_set(G_OBJECT(overlay), "shaded-background", true, nullptr);
	g_object_set(G_OBJECT(time), "silent", !timeVisible, nullptr);

	g_object_set(G_OBJECT(pipeline), "video-sink", bin, nullptr);

	elements << overlay << time;

	timer.setSingleShot(true);
	connect(&timer, SIGNAL(timeout()), SLOT(clear()));
}
