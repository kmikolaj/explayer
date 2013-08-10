#include "videofilter.h"

void VideoFilter::link(VideoFilter *dest)
{
	if (videosink)
	{
		filter->unlink(videosink);
		bin->remove(videosink);
//		delete videosink;
	}
	videosink = dest->videosink;
	if (dest->bin)
	{
		dest->bin->remove(dest->videosink);
//		delete dest->bin;
	}
	dest->bin = bin;
	bin->add(dest->filter);
	bin->add(dest->videosink);
	filter->link(dest->filter);
	dest->filter->link(dest->videosink);
}


void VideoFilter::unlink(VideoFilter *dest)
{

}
