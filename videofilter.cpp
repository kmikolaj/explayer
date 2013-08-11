#include "videofilter.h"

void VideoFilter::link(VideoFilter *dest)
{
	if (videosink)
	{
		bin->remove(videosink);
		videosink.clear();
	}
	videosink = dest->videosink;
	QVector<QGst::ElementPtr> elements = dest->getElements();
	for (int i = 0; i < elements.size(); i++)
	{
		dest->bin->remove(elements[i]);
		bin->add(elements[i]);
	}
	for (int i = 0; i < elements.size(); i++)
	{
		if (i < elements.size() - 1)
			elements[i]->link(elements[i + 1]);
	}
	dest->bin->remove(dest->videosink);
	bin->add(dest->videosink);
	dest->bin.clear();

	getElements().last()->link(dest->getElements().first());
	dest->getElements().last()->link(dest->videosink);
}


void VideoFilter::unlink(VideoFilter *dest)
{

}
