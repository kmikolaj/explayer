#include "videofilter.h"

void VideoFilter::link(VideoFilter *dest)
{
	if (videosink)
	{
		gst_bin_remove(GST_BIN(bin), videosink);
//		videosink.clear();
	}
	videosink = dest->videosink;
	QVector<GstElement*> elements = dest->getElements();
	for (int i = 0; i < elements.size(); i++)
	{
		gst_bin_remove(GST_BIN(dest->bin), elements[i]);
		gst_bin_add(GST_BIN(bin), elements[i]);
	}
	for (int i = 0; i < elements.size(); i++)
	{
		if (i < elements.size() - 1)
			gst_element_link(elements[i], elements[i + 1]);
	}
	gst_bin_remove(GST_BIN(dest->bin), dest->videosink);
	gst_bin_add(GST_BIN(bin), dest->videosink);
//	dest->bin.clear();

	gst_element_link(getElements().last(), dest->getElements().first());
	gst_element_link(dest->getElements().last(), dest->videosink);
}


void VideoFilter::unlink(VideoFilter *dest)
{

}
