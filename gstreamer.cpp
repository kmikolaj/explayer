#include "gstreamer.h"
#include <glib.h>
#include <QDebug>
#include <QTimer>
#include <QPainter>

// temporary, options
bool subtitles = true;
bool aspectratio = true;

Gstreamer::Gstreamer(QWidget *window)
    : QObject(window), surface(window), pipeline(NULL), xoverlay(NULL)
{
	gst_init(NULL, NULL);
	connect(&positionTimer, SIGNAL(timeout()), this, SIGNAL(positionChanged()));

	pipeline = gst_element_factory_make("playbin", "player");

	if (pipeline)
	{
		// add bus
		GstBus *bus;
		bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
		gst_bus_add_watch (bus, Gstreamer::busCallback, this);
		gst_object_unref (bus);

		g_object_set(G_OBJECT(pipeline), "force-aspect-ratio", aspectratio, NULL);

		balance = new Balance(pipeline, this);
	}
}

Gstreamer::~Gstreamer()
{
	if (pipeline)
	{
		gst_element_set_state(pipeline, GST_STATE_NULL);
		gst_object_unref(pipeline);
	}
}

void Gstreamer::setVideo(const QString &path)
{
	if (pipeline)
	{
		video = path;
		QUrl uri = makeUrl(path);
//		surface->setAttribute(Qt::WA_PaintOnScreen, true);
//		surface->setAttribute(Qt::WA_NoSystemBackground, true);
		g_object_set(G_OBJECT(pipeline), "uri", uri.toEncoded().data(), NULL);
	}
}

void Gstreamer::setSubtitles(const QString &path)
{
	if (pipeline)
	{
		sub = path;
		QUrl uri = makeUrl(path);
		if (subtitles)
		{
			g_object_set(G_OBJECT(pipeline), "suburi", uri.toEncoded().data(), NULL);
		}
//		pipeline->setProperty("subtitle-encoding", enc);
//		pipeline->setProperty("subtitle-font-desc", font);
	}
}

bool Gstreamer::embed(QWidget *window)
{
	// TODO
	return true;
}

GstState Gstreamer::state()
{
	GstState state = GST_STATE_NULL;
	if (pipeline)
	{
		gst_element_get_state(pipeline, &state, NULL, -1);
	}
	return state;
}

gboolean Gstreamer::busCallback(GstBus *bus, GstMessage *msg, gpointer args)
{
	Q_UNUSED(bus);
	Gstreamer* gst = (Gstreamer*)args;
	switch (GST_MESSAGE_TYPE(msg))
	{
	case GST_MESSAGE_STATE_CHANGED:
	{
		if (msg->src == GST_OBJECT(gst->pipeline))
		{
			handlePipelineStateChange(gst, msg);
		}
		break;
	}
	case GST_MESSAGE_ERROR:
	{
		gchar *debug;
		GError *err;

		gst_message_parse_error(msg, &err, &debug);
		qCritical("[%s]: %s %s", GST_OBJECT_NAME(msg->src), err->message,
		          debug);
		g_free(debug);
		g_error_free(err);

		gst->stop();

		break;
	}
	case GST_MESSAGE_ELEMENT:
	{
		if (gst_structure_has_name(gst_message_get_structure(msg), "prepare-window-handle"))
		{
			WId xwinid = gst->surface->winId();
			if (xwinid != 0)
			{
//				Gstreamer::Instance()->surface->setAttribute(Qt::WA_PaintOnScreen, true);
//				Gstreamer::Instance()->surface->setAttribute(Qt::WA_NoSystemBackground, true);
			    gst->xoverlay = GST_VIDEO_OVERLAY(GST_MESSAGE_SRC(msg));
			    gst_video_overlay_set_window_handle(gst->xoverlay, xwinid);
			}
		}

		break;
	}
	case GST_MESSAGE_EOS:
	{
		gst->stop();
		break;
	}
	default:
		break;
	}

	return true;
}

void Gstreamer::handlePipelineStateChange(Gstreamer *gst, GstMessage *msg)
{
	GstState old_state, new_state;
	gst_message_parse_state_changed (msg, &old_state, &new_state, NULL);

	switch(new_state)
	{
	case GST_STATE_PLAYING:
		//start the timer when the pipeline starts playing
//		positionTimer.start(int(1000.0 / meta.getFramerate()));
		break;
	case GST_STATE_PAUSED:
		//stop the timer when the pipeline pauses
		if (old_state == GST_STATE_PLAYING)
		{
			gst->positionTimer.stop();
		}
//		else if (old_state == GST_STATE_READY && !startingPosition.Changed)
//		{
//			setPosition(startingPosition.Time);
//			startingPosition.Changed = true;
//		}
		else if (old_state == GST_STATE_READY)
		{
			if (!gst->osd)
			{
				gst->osd = new Osd(gst->pipeline, gst);
				gst->osd->enable();
			}
		}
		break;
    case GST_STATE_READY:
		break;
    case GST_STATE_NULL:
		gst->positionTimer.stop();
		break;
	default:
		break;
	}

	emit gst->stateChanged();
	// if new == NULL releasesink()
}

void Gstreamer::setHardwareAcceleration(bool enable)
{
	GstElementFactory *plugin;
	plugin = gst_element_factory_find("vaapidecode");
	if (plugin)
	{
		gst_plugin_feature_set_rank(GST_PLUGIN_FEATURE(plugin), enable ? GST_RANK_PRIMARY : GST_RANK_NONE);
	}
	plugin = gst_element_factory_find("vaapisink");
	if (plugin)
	{
		gst_plugin_feature_set_rank(GST_PLUGIN_FEATURE(plugin), enable ? GST_RANK_PRIMARY : GST_RANK_NONE);
	}
}

void Gstreamer::refresh()
{
	surface->setAttribute(Qt::WA_PaintOnScreen, false);
	surface->setAttribute(Qt::WA_NoSystemBackground, false);
}

QUrl Gstreamer::makeUrl(const QString &path)
{
	return QUrl::fromUserInput(path);
}

void Gstreamer::play()
{
	if (pipeline)
	{
		gst_element_set_state(pipeline, GST_STATE_PLAYING);
		if (osd)
			osd->setText("▶");
	}
}

void Gstreamer::play(QTime position)
{
	if (pipeline)
	{
		gst_element_set_state(pipeline, GST_STATE_PLAYING);
//		startingPosition.Time = position;
//		startingPosition.Changed = false;
//		osd->setText("▶");
	}
}

void Gstreamer::pause()
{
	if (pipeline)
	{
		//osd->clear();
		//osd->setText("■"); nie działa
		gst_element_set_state(pipeline, GST_STATE_PAUSED);
	}
}

void Gstreamer::toggle()
{
	switch (state())
	{
	case GST_STATE_PLAYING:
		pause();
		break;
	case GST_STATE_PAUSED:
		play();
		break;
	default:
		break;
	}
}

void Gstreamer::stop()
{
	if (pipeline)
	{
		gst_element_set_state(pipeline, GST_STATE_NULL);
		refresh();
		emit stateChanged();
	}
}

void Gstreamer::setVolume(double volume)
{
	if (pipeline)
	{
		GstStreamVolume* vptr = GST_STREAM_VOLUME(pipeline);

		if (vptr)
		{
			double dvolume = gst_stream_volume_convert_volume(GST_STREAM_VOLUME_FORMAT_LINEAR,
			                GST_STREAM_VOLUME_FORMAT_CUBIC, volume);
			gst_stream_volume_set_volume(vptr, GST_STREAM_VOLUME_FORMAT_CUBIC, dvolume);
//			osd->setText("Volume: " + QString::number(int(volume * 100)));
		}
	}
}

void Gstreamer::setHue(double hue)
{
	if (pipeline)
	{
		GstColorBalanceChannel *channel = balance->Channel("hue");
		if (channel)
		{
			int value = int(hue * double(hue > 0 ? channel->max_value : channel->min_value));
			gst_color_balance_set_value(balance->Get(), channel, value);
		}
	}
}

void Gstreamer::setSaturation(double saturation)
{
	if (pipeline)
	{
		GstColorBalanceChannel *channel = balance->Channel("saturation");
		if (channel)
		{
			int value = int(saturation * double(saturation > 0 ? channel->max_value : channel->min_value));
			gst_color_balance_set_value(balance->Get(), channel, value);
		}
	}
}

void Gstreamer::setBrightness(double brightness)
{
	if (pipeline)
	{
		GstColorBalanceChannel *channel = balance->Channel("brightness");
		if (channel)
		{
			int value = int(brightness * double(brightness > 0 ? channel->max_value : channel->min_value));
			gst_color_balance_set_value(balance->Get(), channel, value);
		}
	}
}

void Gstreamer::setContrast(double contrast)
{
	if (pipeline)
	{
		GstColorBalanceChannel *channel = balance->Channel("contrast");
		if (channel)
		{
			int value = int(contrast * double(contrast > 0 ? channel->max_value : channel->min_value));
			gst_color_balance_set_value(balance->Get(), channel, value);
		}
	}
}

void Gstreamer::forceaspectratio()
{
	if (pipeline)
	{
		aspectratio = !aspectratio;
		g_object_set(G_OBJECT(pipeline), "force-aspect-ratio", aspectratio, NULL);
//		if (aspectratio)
//		{
//			osd->setText("keep aspect ratio");
//		}
//		else
//		{
//			osd->setText("ignore aspect ratio");
//		}
//		update();
	}
}

void Gstreamer::togglesubtitles()
{
	// does not work
	// https://bugzilla.gnome.org/show_bug.cgi?id=589515
	if (pipeline)
	{
		if (subtitles)
		{
//			osd->setText("no subtitles");
			g_object_set(G_OBJECT(pipeline), "suburi", NULL, NULL);
		}
		else
		{
//			osd->setText("subtitles");
			QUrl uri = makeUrl(sub);
			g_object_set(G_OBJECT(pipeline), "suburi", uri.toEncoded().data(), NULL);
		}
		subtitles = !subtitles;
	}
}

void Gstreamer::toggletime()
{
	if (pipeline && osd)
	{
		osd->toggleTime();
	}
}

void Gstreamer::mute()
{
	if (pipeline)
	{
		GstStreamVolume* volume = GST_STREAM_VOLUME(pipeline);
		if (volume)
		{
			bool mute = gst_stream_volume_get_mute(volume);
			gst_stream_volume_set_mute(volume, !mute);
			//osd->setText(mute ? "unmute" : "mute");
		}
	}
}

void Gstreamer::seek(const UTime &pos)
{
	if (pipeline)
	{
//		gst_element_get_state(pipeline, NULL, NULL, 100 * GST_MSECOND);
		gint64 ms = gint64(pos.Msec);
		gst_element_seek(pipeline, 1.0, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH, GST_SEEK_TYPE_SET, ms * GST_MSECOND, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE);
	}
}

UTime Gstreamer::position() const
{
	GstFormat fmt = GST_FORMAT_TIME;
	gint64 value = 0;
	if (gst_element_query_position(pipeline, fmt, &value))
		return UTime(qint64(value));
	else
		return UTime();
}

UTime Gstreamer::duration() const
{
	GstFormat fmt = GST_FORMAT_TIME;
	gint64 value = 0;
	if (gst_element_query_duration(pipeline, fmt, &value))
		return UTime(qint64(value));
	else
		return UTime();
}

double Gstreamer::volume() const
{
	if (pipeline)
	{
		GstStreamVolume* vptr = GST_STREAM_VOLUME(pipeline);

		if (vptr)
		{
			return gst_stream_volume_get_volume(vptr, GST_STREAM_VOLUME_FORMAT_CUBIC);
		}
	}
	return 0.0;
}

double Gstreamer::hue() const
{
	if (pipeline)
	{
		GstColorBalanceChannel *channel = balance->Channel("hue");
		if (channel)
		{
			int value = gst_color_balance_get_value(balance->Get(), channel);
			return double(value) / double(value > 0 ? channel->max_value : channel->min_value);
		}
	}
	return 0.0;
}

double Gstreamer::saturation() const
{
	if (pipeline)
	{
		GstColorBalanceChannel *channel = balance->Channel("saturation");
		if (channel)
		{
			int value = gst_color_balance_get_value(balance->Get(), channel);
			return double(value) / double(value > 0 ? channel->max_value : channel->min_value);
		}
	}
	return 0.0;
}

double Gstreamer::brightness() const
{
	if (pipeline)
	{
		GstColorBalanceChannel *channel = balance->Channel("brightness");
		if (channel)
		{
			int value = gst_color_balance_get_value(balance->Get(), channel);
			return double(value) / double(value > 0 ? channel->max_value : channel->min_value);
		}
	}
	return 0.0;
}

double Gstreamer::contrast() const
{
	if (pipeline)
	{
		GstColorBalanceChannel *channel = balance->Channel("contrast");
		if (channel)
		{
			int value = gst_color_balance_get_value(balance->Get(), channel);
			return double(value) / double(value > 0 ? channel->max_value : channel->min_value);
		}
	}
	return 0.0;
}

void Gstreamer::setPosition(const UTime &pos, Gstreamer::SeekFlag flag)
{
	if (pipeline)
	{
		//
	}
}

void Gstreamer::setVideoArea(const QRect &rect)
{
	if (GST_IS_VIDEO_OVERLAY(xoverlay))
	{
		gst_video_overlay_set_render_rectangle(GST_VIDEO_OVERLAY(xoverlay), rect.left(), rect.top(), rect.width(),rect.height());
	}
}

void Gstreamer::expose()
{
	if (state() == GST_STATE_PLAYING || state() == GST_STATE_PAUSED)
	{
		if (GST_IS_VIDEO_OVERLAY(xoverlay))
		{
			gst_video_overlay_expose(xoverlay);
		}
	}
	else
	{
		QPainter p(surface);
		p.fillRect(surface->rect(), Qt::black);
	}
}
