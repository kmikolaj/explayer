#ifndef DISCOVERER_H
#define DISCOVERER_H

#include <gst/gst.h>
#include <gst/pbutils/gstdiscoverer.h>
#include "gstreamer.h"

class Discoverer
{
public:
	static void on_discovered(GstDiscoverer *discoverer, GstDiscovererInfo *info, GError *err, Gstreamer *gst);
	static void on_finished(GstDiscoverer *discoverer, Metadata **meta);

private:
	static void fillTags(const GstTagList *tags, const gchar *tag, gpointer args);
	static QString convert(const GValue &val);
};

#endif // DISCOVERER_H
