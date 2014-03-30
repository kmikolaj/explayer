#include "gstreamer.h"
#include <glib.h>
#include <QDebug>
#include <QTimer>
#include <QPainter>

// temporary, options
bool subtitles = true;
bool aspectratio = true;
bool hasosd = true;
bool hasbalance = true;

Gstreamer::Gstreamer(QWidget *window)
    : PlayerInterface(window), surface(window), pipeline(nullptr), xoverlay(nullptr),
      osd(nullptr), balance(nullptr)
{
	gst_init(nullptr, nullptr);
	connect(&positionTimer, SIGNAL(timeout()), this, SIGNAL(positionChanged()));

#if GST_VERSION_MAJOR == 1
	pipeline = gst_element_factory_make("playbin", "player");
#else
	pipeline = gst_element_factory_make("playbin2", "player");
#endif

	if (pipeline)
	{
		osd = new Osd(pipeline, this);
		if (hasosd)
			osd->enable();

		WId xwinid = surface->winId();
		if (xwinid != 0)
#if GST_VERSION_MAJOR == 1
		    gst_video_overlay_set_window_handle(GST_VIDEO_OVERLAY(pipeline), xwinid);
#else
			gst_x_overlay_set_window_handle(GST_X_OVERLAY(pipeline), xwinid);
#endif

		// add bus
		GstBus *bus;
		bus = gst_pipeline_get_bus(GST_PIPELINE (pipeline));
		gst_bus_add_watch(bus, Gstreamer::busCallback, this);
		gst_object_unref(bus);

		g_object_set(G_OBJECT(pipeline), "force-aspect-ratio", aspectratio, nullptr);

		if (hasbalance)
			balance = new Balance(pipeline, this);
	}

//	surface->setAttribute(Qt::WA_PaintOnScreen, true);
//	surface->setAttribute(Qt::WA_NoSystemBackground, true);
	setHardwareAcceleration(false);
//			gst_element_set_state(pipeline, GST_STATE_READY);
}

Gstreamer::~Gstreamer()
{
	if (pipeline)
	{
		gst_element_set_state(pipeline, GST_STATE_NULL);
		gst_object_unref(pipeline);
	}
}
// FIX
void Gstreamer::setVideo(const QString &path)
{
	if (pipeline)
	{
		video = path;
		QUrl uri = makeUrl(path);
//		surface->setAttribute(Qt::WA_PaintOnScreen, true);
//		surface->setAttribute(Qt::WA_NoSystemBackground, true);
		g_object_set(G_OBJECT(pipeline), "uri", uri.toEncoded().data(), nullptr);
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
			g_object_set(G_OBJECT(pipeline), "suburi", uri.toEncoded().data(), nullptr);
		}
	}
}

void Gstreamer::setFont(const QString &font, const QString &enc)
{
	if (pipeline)
	{
		g_object_set(G_OBJECT(pipeline), "subtitle-encoding", qPrintable(enc), nullptr);
		g_object_set(G_OBJECT(pipeline), "subtitle-font-desc", qPrintable(font), nullptr);
	}
}
// FIX
void Gstreamer::setPosition(const QTime &pos)
{
//	if (player->metadata().getTags().containerFormat() == "ASF")
//		player->setPosition(newPos, Accurate);
	seek(UTime(pos));
}

void Gstreamer::setPosition(const qint32 frame)
{
	seek(UTime(frame), SeekFlag(Accurate | Skip));
}

GstState Gstreamer::state()
{
	GstState state = GST_STATE_NULL;
	if (pipeline)
	{
		gst_element_get_state(pipeline, &state, nullptr, -1);
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
// FIX
void Gstreamer::handlePipelineStateChange(Gstreamer *gst, GstMessage *msg)
{
	GstState old_state, new_state;
	gst_message_parse_state_changed (msg, &old_state, &new_state, nullptr);
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
	// if new == nullptr releasesink()
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

bool Gstreamer::canSeek()
{
	GstState currentState = state();
	return (currentState != GST_STATE_READY && currentState != GST_STATE_NULL);
}
/*
void Gstreamer::paintEvent(QPaintEvent *event)
{
	setVideoArea(surface->rect());
	expose();
}
*/
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
		{
			osd->setText("▶");
		}
	}
}
// FIX
void Gstreamer::play(QTime position)
{
	if (pipeline)
	{
		gst_element_set_state(pipeline, GST_STATE_PLAYING);
//		startingPosition.Time = position;
//		startingPosition.Changed = false;
		if (osd)
		{
			osd->setText("▶");
		}
	}
}

void Gstreamer::pause()
{
	if (pipeline)
	{
		if (osd)
		{
			osd->clear();
		}
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
// FIX
void Gstreamer::setVolume(double volume)
{
//	if (vol > 0.0)
//	{
//		double linear = QGst::StreamVolume::convert(QGst::StreamVolumeFormatCubic, QGst::StreamVolumeFormatLinear, vol);
//		double newVol = linear - MIN(linear, 0.05);
//		player->setVolume(newVol);
//		ui->controls->setVolume(newVol * 100);
//	}

//	if (vol < 1.0)
//	{
//		double linear = QGst::StreamVolume::convert(QGst::StreamVolumeFormatCubic, QGst::StreamVolumeFormatLinear, vol);
//		double newVol = linear + MIN(1.0 - linear, 0.05);
//		player->setVolume(newVol);
//		ui->controls->setVolume(newVol * 100);
//	}

	if (pipeline)
	{
		GstStreamVolume* vptr = GST_STREAM_VOLUME(pipeline);
		if (vptr)
		{
			double dvolume = gst_stream_volume_convert_volume(GST_STREAM_VOLUME_FORMAT_LINEAR,
			                GST_STREAM_VOLUME_FORMAT_CUBIC, volume);
			gst_stream_volume_set_volume(vptr, GST_STREAM_VOLUME_FORMAT_CUBIC, dvolume);
			if (osd)
			{
				osd->setText("Volume: " + QString::number(int(volume * 100)));
			}
		}
	}
}

void Gstreamer::setHue(double hue)
{
	if (pipeline && balance)
	{
		GstColorBalanceChannel *channel = balance->Channel("hue");
		if (channel)
		{
			int value = int(hue * double(hue > 0 ? channel->max_value : channel->min_value));
			gst_color_balance_set_value(*balance, channel, value);
		}
	}
}

void Gstreamer::setSaturation(double saturation)
{
	if (pipeline && balance)
	{
		GstColorBalanceChannel *channel = balance->Channel("saturation");
		if (channel)
		{
			int value = int(saturation * double(saturation > 0 ? channel->max_value : channel->min_value));
			gst_color_balance_set_value(*balance, channel, value);
		}
	}
}

void Gstreamer::setBrightness(double brightness)
{
	if (pipeline && balance)
	{
		GstColorBalanceChannel *channel = balance->Channel("brightness");
		if (channel)
		{
			int value = int(brightness * double(brightness > 0 ? channel->max_value : channel->min_value));
			gst_color_balance_set_value(*balance, channel, value);
		}
	}
}

void Gstreamer::setContrast(double contrast)
{
	if (pipeline && balance)
	{
		GstColorBalanceChannel *channel = balance->Channel("contrast");
		if (channel)
		{
			int value = int(contrast * double(contrast > 0 ? channel->max_value : channel->min_value));
			gst_color_balance_set_value(*balance, channel, value);
		}
	}
}

void Gstreamer::forceaspectratio()
{
	if (pipeline)
	{
		aspectratio = !aspectratio;
		g_object_set(G_OBJECT(pipeline), "force-aspect-ratio", aspectratio, nullptr);
		if (aspectratio)
		{
			osd->setText("keep aspect ratio");
		}
		else
		{
			osd->setText("ignore aspect ratio");
		}
		update();
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
			g_object_set(G_OBJECT(pipeline), "suburi", nullptr, nullptr);
		}
		else
		{
//			osd->setText("subtitles");
			QUrl uri = makeUrl(sub);
			g_object_set(G_OBJECT(pipeline), "suburi", uri.toEncoded().data(), nullptr);
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
			if (osd)
			{
				osd->setText(mute ? "unmute" : "mute");
			}
		}
	}
}

UTime Gstreamer::position() const
{
	GstFormat fmt = GST_FORMAT_TIME;
	gint64 value = 0;
#if GST_VERSION_MAJOR == 1
	if (gst_element_query_position(pipeline, fmt, &value))
#else
	if (gst_element_query_position(pipeline, &fmt, &value))
#endif
		return UTime(qint64(value));
	else
		return UTime();
}

UTime Gstreamer::duration() const
{
	GstFormat fmt = GST_FORMAT_TIME;
	gint64 value = 0;
#if GST_VERSION_MAJOR == 1
	if (gst_element_query_duration(pipeline, fmt, &value))
#else
	if (gst_element_query_duration(pipeline, &fmt, &value))
#endif
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
	if (pipeline && balance)
	{
		GstColorBalanceChannel *channel = balance->Channel("hue");
		if (channel)
		{
			int value = gst_color_balance_get_value(*balance, channel);
			return double(value) / double(value > 0 ? channel->max_value : channel->min_value);
		}
	}
	return 0.0;
}

double Gstreamer::saturation() const
{
	if (pipeline && balance)
	{
		GstColorBalanceChannel *channel = balance->Channel("saturation");
		if (channel)
		{
			int value = gst_color_balance_get_value(*balance, channel);
			return double(value) / double(value > 0 ? channel->max_value : channel->min_value);
		}
	}
	return 0.0;
}

double Gstreamer::brightness() const
{
	if (pipeline && balance)
	{
		GstColorBalanceChannel *channel = balance->Channel("brightness");
		if (channel)
		{
			int value = gst_color_balance_get_value(*balance, channel);
			return double(value) / double(value > 0 ? channel->max_value : channel->min_value);
		}
	}
	return 0.0;
}

double Gstreamer::contrast() const
{
	if (pipeline && balance)
	{
		GstColorBalanceChannel *channel = balance->Channel("contrast");
		if (channel)
		{
			int value = gst_color_balance_get_value(*balance, channel);
			return double(value) / double(value > 0 ? channel->max_value : channel->min_value);
		}
	}
	return 0.0;
}
// FIX
void Gstreamer::seek(const UTime &pos, Gstreamer::SeekFlag flag)
{
	if (pipeline)
	{
//		gst_element_get_state(pipeline, nullptr, nullptr, 100 * GST_MSECOND);
		GstSeekFlags flags = GstSeekFlags(GST_SEEK_FLAG_FLUSH | flag);
//		GST_SEEK_FLAG_ACCURATE
//		GST_SEEK_FLAG_SKIP
		gint64 ms = gint64(pos.Msec);
		gst_element_seek(pipeline, 1.0, GST_FORMAT_TIME, flags, GST_SEEK_TYPE_SET, ms * GST_MSECOND, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE);
	}
}

void Gstreamer::setVideoArea(const QRect &rect)
{
#if GST_VERSION_MAJOR == 1
	if (GST_IS_VIDEO_OVERLAY(xoverlay))
	{
		gst_video_overlay_set_render_rectangle(GST_VIDEO_OVERLAY(xoverlay), rect.left(), rect.top(), rect.width(),rect.height());
	}
#else
	if (GST_IS_X_OVERLAY(xoverlay))
	{
		gst_x_overlay_set_render_rectangle(GST_X_OVERLAY(xoverlay), rect.left(), rect.top(), rect.width(),rect.height());
	}
#endif
}

void Gstreamer::expose()
{
	if (state() == GST_STATE_PLAYING || state() == GST_STATE_PAUSED)
	{
#if GST_VERSION_MAJOR == 1
		if (GST_IS_VIDEO_OVERLAY(xoverlay))
		{
			gst_video_overlay_expose(xoverlay);
		}
#else
		if (GST_IS_X_OVERLAY(xoverlay))
		{
			gst_x_overlay_expose(xoverlay);
		}
#endif
	}
	else
	{
		QPainter p(surface);
		p.fillRect(surface->rect(), Qt::black);
	}
}
