#include "gstreamer.h"
#include <glib.h>
#include <QDebug>
#include <QTimer>
#include <QFileInfo>
#include "discoverer.h"

// temporary, options
bool subtitles = true;
bool aspectratio = true;
bool hasosd = true;
bool hasbalance = true;
bool hasharddec = false;

Gstreamer::Gstreamer(QWidget *window)
	: PlayerInterface(window), pipeline(nullptr), xoverlay(nullptr), discoverer(nullptr),
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
#if GST_VERSION_MAJOR != 1
		// no idea why it isn't working in gst-1.0
		osd = new Osd(pipeline, this);
		if (hasosd)
			osd->enable();
#endif

		xwinid = surface->winId();
		setOverlay();

		// add bus
		GstBus *bus;
		bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
		gst_bus_add_watch(bus, Gstreamer::busCallback, this);
		gst_object_unref(bus);

#if GST_VERSION_MAJOR == 1
		g_object_set(G_OBJECT(pipeline), "force-aspect-ratio", aspectratio, nullptr);
#endif
		discoverer = gst_discoverer_new(5 * GST_SECOND, nullptr);

		if (discoverer)
		{
			g_signal_connect(discoverer, "discovered", G_CALLBACK(Discoverer::on_discovered), this);
			g_signal_connect(discoverer, "finished", G_CALLBACK(Discoverer::on_finished), &metadata);
			gst_discoverer_start(discoverer);
		}

		if (hasbalance)
			balance = new Balance(pipeline, this);
	}
	setHardwareAcceleration(hasharddec);
}

Gstreamer::~Gstreamer()
{
	if (pipeline)
	{
		gst_element_set_state(pipeline, GST_STATE_NULL);
		gst_object_unref(pipeline);
	}

	if (discoverer)
	{
		gst_discoverer_stop(discoverer);
		g_object_unref(discoverer);
	}

	delete balance;
	delete osd;
}
// FIX
void Gstreamer::setVideo(const QString &path)
{
	if (pipeline)
	{
		QUrl uri = makeUrl(path);
		g_object_set(G_OBJECT(pipeline), "uri", uri.toEncoded().data(), nullptr);

		// discoverer
		if (!gst_discoverer_discover_uri_async(discoverer, uri.toEncoded().data()))
		{
			qDebug() << "discovery fail";
		}
	}
}

void Gstreamer::setSubtitles(const QString &path)
{
	if (pipeline)
	{
		QUrl uri = makeUrl(path);
		sub = uri;
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

void Gstreamer::setPosition(const QTime &pos)
{
	const QMap<QString, QString> &tags = getMetadata()->getTags();
	bool special = false;
	if (tags.contains("container-format"))
	{
		if (tags["container-format"] == "ASF")
			special = true;
	}
	special ? seek(UTime(pos), GST_SEEK_FLAG_ACCURATE) : seek(UTime(pos));
}

void Gstreamer::setPosition(const qint32 frame)
{
	seek(UTime(frame), GstSeekFlags(GST_SEEK_FLAG_ACCURATE | GST_SEEK_FLAG_SKIP));
}

GstState Gstreamer::state() const
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
	Gstreamer *gst = (Gstreamer *)args;
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
#if GST_VERSION_MAJOR != 1
		if (gst_structure_has_name(gst_message_get_structure(msg), "prepare-xwindow-id"))
		{
			if (gst->xwinid != 0)
			{
				gst->xoverlay = GST_X_OVERLAY(GST_MESSAGE_SRC(msg));
				gst_x_overlay_set_window_handle(gst->xoverlay, gst->xwinid);
			}
		}
#endif
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
	gst_message_parse_state_changed(msg, &old_state, &new_state, nullptr);
	switch (new_state)
	{
	case GST_STATE_PLAYING:
		//start the timer when the pipeline starts playing
		gst->positionTimer.start(int(1000.0 / gst->metadata->getFramerate(1)));
		emit gst->stateChanged(PLAYING);
		break;
	case GST_STATE_PAUSED:
		//stop the timer when the pipeline pauses
		if (old_state == GST_STATE_PLAYING)
		{
			gst->positionTimer.stop();
			emit gst->positionChanged();
		}
		else if (old_state == GST_STATE_READY)
		{
			emit gst->volumeChanged((gst->volume() + 0.005) * 100.0);
		}
//		else if (old_state == GST_STATE_READY && !startingPosition.Changed)
//		{
//			setPosition(startingPosition.Time);
//			startingPosition.Changed = true;
//		}
		emit gst->stateChanged(PAUSED);
		break;
	case GST_STATE_READY:
		break;
	case GST_STATE_NULL:
		gst->positionTimer.stop();
		emit gst->stateChanged(STOPPED);
		break;
	default:
		break;
	}
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

bool Gstreamer::isSeekable() const
{
	GstState currentState = state();
	return (currentState != GST_STATE_READY && currentState != GST_STATE_NULL);
}

QUrl Gstreamer::makeUrl(const QString &path)
{
	QRegExp re("[^/]*");
	if (re.exactMatch(path))
		return QUrl::fromUserInput(QFileInfo(path).absoluteFilePath());
	else
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
		emit stateChanged(STOPPED);
	}
}

void Gstreamer::setVolume(double volume)
{
	volume = MIN(MAX(volume, 0.0), 1.0);
	if (pipeline)
	{
		GstStreamVolume *vptr = GST_STREAM_VOLUME(pipeline);
		if (vptr)
		{
			gst_stream_volume_set_volume(vptr, GST_STREAM_VOLUME_FORMAT_CUBIC, volume);
			double dvolume = (volume + 0.005) * 100.0;
			emit volumeChanged(dvolume);
			if (osd)
			{
				osd->setText("Volume: " + QString::number(int(dvolume)));
			}
		}
	}
}

void Gstreamer::setHue(double hue)
{
	setBalanceChannel("hue", hue);
}

void Gstreamer::setSaturation(double saturation)
{
	setBalanceChannel("saturation", saturation);
}

void Gstreamer::setBrightness(double brightness)
{
	setBalanceChannel("brightness", brightness);
}

void Gstreamer::setContrast(double contrast)
{
	setBalanceChannel("contrast", contrast);
}

void Gstreamer::forceaspectratio()
{
	if (pipeline)
	{
		aspectratio = !aspectratio;
#if GST_VERSION_MAJOR == 1
		g_object_set(G_OBJECT(pipeline), "force-aspect-ratio", aspectratio, nullptr);
#else
		GstElement *videosink = nullptr;
		g_object_get(G_OBJECT(pipeline), "video-sink", &videosink, NULL);
		g_object_set(G_OBJECT(videosink), "force-aspect-ratio", aspectratio, nullptr);
#endif
		if (aspectratio)
		{
			if (osd)
				osd->setText("keep aspect ratio");
		}
		else
		{
			if (osd)
				osd->setText("ignore aspect ratio");
		}
		// redraw black bars
		surface->update();
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
			if (osd)
				osd->setText("no subtitles");
			g_object_set(G_OBJECT(pipeline), "suburi", nullptr, nullptr);
		}
		else
		{
			if (osd)
				osd->setText("subtitles");
			g_object_set(G_OBJECT(pipeline), "suburi", sub.toEncoded().data(), nullptr);
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
		GstStreamVolume *volume = GST_STREAM_VOLUME(pipeline);
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

void Gstreamer::on_metadata_update()
{
	UTime::setFps(getMetadata()->getFramerate(1));
	emit videoChanged();
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
		GstStreamVolume *vptr = GST_STREAM_VOLUME(pipeline);
		if (vptr)
		{
			return gst_stream_volume_get_volume(vptr, GST_STREAM_VOLUME_FORMAT_CUBIC);
		}
	}
	return 0.0;
}

double Gstreamer::hue() const
{
	return getBalanceChannel("hue");
}

double Gstreamer::saturation() const
{
	return getBalanceChannel("saturation");
}

double Gstreamer::brightness() const
{
	return getBalanceChannel("brightness");
}

double Gstreamer::contrast() const
{
	return getBalanceChannel("contrast");
}

void Gstreamer::seek(const UTime &pos, GstSeekFlags flags)
{
	if (pipeline)
	{
		GstSeekFlags flag = GstSeekFlags(GST_SEEK_FLAG_FLUSH | flags);
		gint64 ns = gint64(pos.Nsec);
		gst_element_seek(pipeline, 1.0, GST_FORMAT_TIME, flag, GST_SEEK_TYPE_SET, ns, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE);
	}
}

void Gstreamer::setBalanceChannel(const QString &name, double value)
{
	if (pipeline && balance)
	{
		GstColorBalanceChannel *channel = balance->Channel(name);
		if (channel)
		{
			value = MIN(MAX(value, -1.0), 1.0);
			int newValue;
			if (value > 0.0)
				newValue = int(value * double(channel->max_value) + 0.05);
			else
				newValue = int(value * double(-channel->min_value) - 0.05);
			gst_color_balance_set_value(*balance, channel, newValue);
		}
	}
}

double Gstreamer::getBalanceChannel(const QString &name) const
{
	if (pipeline && balance)
	{
		GstColorBalanceChannel *channel = balance->Channel(name);
		if (channel)
		{
			int value = gst_color_balance_get_value(*balance, channel);
			return double(value) / double(value > 0.0 ? channel->max_value : -channel->min_value);
		}
	}
	return 0.0;
}

void Gstreamer::setVideoArea(const QRect &rect)
{
#if GST_VERSION_MAJOR == 1
	if (GST_IS_VIDEO_OVERLAY(xoverlay))
	{
		gst_video_overlay_set_render_rectangle(GST_VIDEO_OVERLAY(xoverlay), rect.left(), rect.top(), rect.width(), rect.height());
	}
#else
	if (GST_IS_X_OVERLAY(xoverlay))
	{
		gst_x_overlay_set_render_rectangle(GST_X_OVERLAY(xoverlay), rect.left(), rect.top(), rect.width(), rect.height());
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
}

void Gstreamer::setOverlay()
{
#if GST_VERSION_MAJOR == 1
	if (xwinid != 0)
		gst_video_overlay_set_window_handle(GST_VIDEO_OVERLAY(pipeline), xwinid);
#else
	if (hasharddec && xwinid != 0)
		gst_x_overlay_set_window_handle(GST_X_OVERLAY(pipeline), xwinid);
#endif
}
