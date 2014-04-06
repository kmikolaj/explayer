#include "discoverer.h"
#include <QUrl>
#include <QFile>

void Discoverer::fillTags(const GstTagList *tags, const gchar *tag, gpointer args)
{
	GValue val = { 0, };
	Metadata *meta = (Metadata *)args;

	gst_tag_list_copy_value(&val, tags, tag);
	meta->addTag(tag, convert(val));
	g_value_unset(&val);
}

void Discoverer::on_discovered(GstDiscoverer *discoverer, GstDiscovererInfo *info, GError *err, Gstreamer *gst)
{
	Q_UNUSED(err);
	Q_UNUSED(discoverer);

	Metadata *meta = gst->getMetadata();
	delete meta;

	GstDiscovererResult result;
	result = gst_discoverer_info_get_result(info);
	if (result != GST_DISCOVERER_OK)
	{
		gst->setMetadata(nullptr);
		return;
	}

	meta = new Metadata();

	// is seekable
	meta->setSeekable(gst_discoverer_info_get_seekable(info));
	// extract duration
	qint64 time = gst_discoverer_info_get_duration(info);
	meta->setDuration(UTime(time));
	// extract filename
	QUrl uri(gst_discoverer_info_get_uri(info));
	meta->setFilename(uri.toLocalFile());
	// set filesize
	QFile file(uri.toLocalFile());
	meta->setSize(file.size());

	// extract tags
	const GstTagList *tags;
	tags = gst_discoverer_info_get_tags(info);
	if (tags)
	{
		gst_tag_list_foreach(tags, fillTags, meta);
	}

	GList *list, *iterator;

	// extract video streams
	iterator = list = gst_discoverer_info_get_video_streams(info);
	while (iterator)
	{
		Video video;
		GstDiscovererVideoInfo *vInfo = (GstDiscovererVideoInfo *)iterator->data;
		video.Width = gst_discoverer_video_info_get_width(vInfo);
		video.Height = gst_discoverer_video_info_get_height(vInfo);
		video.Framerate = double(gst_discoverer_video_info_get_framerate_num(vInfo)) /
		                  double(gst_discoverer_video_info_get_framerate_denom(vInfo));
		meta->addVideo(video);
		iterator = iterator->next;
	}
	gst_discoverer_stream_info_list_free(list);

	// extract audio streams
	iterator = list = gst_discoverer_info_get_audio_streams(info);
	while (iterator)
	{
		Audio audio;
		GstDiscovererAudioInfo *aInfo = (GstDiscovererAudioInfo *)iterator->data;
		audio.Channels = gst_discoverer_audio_info_get_channels(aInfo);
		audio.SampleRate = gst_discoverer_audio_info_get_sample_rate(aInfo);
		audio.Language = gst_discoverer_audio_info_get_language(aInfo);
		meta->addAudio(audio);
		iterator = iterator->next;
	}
	gst_discoverer_stream_info_list_free(list);

	// extract subtitle streams
	iterator = list = gst_discoverer_info_get_subtitle_streams(info);
	while (iterator)
	{
		Subtitle subtitle;
		GstDiscovererSubtitleInfo *sInfo = (GstDiscovererSubtitleInfo *)iterator->data;
		gst_discoverer_subtitle_info_get_language(sInfo);
		subtitle.Language = gst_discoverer_subtitle_info_get_language(sInfo);
		meta->addSubtitles(subtitle);
		iterator = iterator->next;
	}
	gst_discoverer_stream_info_list_free(list);

	// fill metadata
	gst->setMetadata(meta);
	QObject::connect(gst->getMetadata(), SIGNAL(updated()), gst, SLOT(on_metadata_update()));
}

void Discoverer::on_finished(GstDiscoverer *discoverer, Metadata **meta)
{
	Q_UNUSED(discoverer);
	emit(*meta)->updated();
}

QString Discoverer::convert(const GValue &val)
{
	gchar *str;
	if (G_VALUE_HOLDS_STRING(&val))
		str = g_value_dup_string(&val);
	else
		str = gst_value_serialize(&val);
	QString tag(str);
	g_free(str);

	return tag;
}
