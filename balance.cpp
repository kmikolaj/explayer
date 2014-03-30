#include "balance.h"

Balance::Balance(GstElement *pipeline, QObject *parent)
	: QObject(parent), balance(GST_COLOR_BALANCE(pipeline))
{
	const GList *controls = gst_color_balance_list_channels(balance);
	const GList *item;
	GstColorBalanceChannel *channel;
	gint index;

	if (controls)
	{
		for (item = controls, index = 0; item != nullptr; item = item->next, ++index)
		{
			channel = (GstColorBalanceChannel *)item->data;
			channels[QString(channel->label).toLower()] = channel;
		}
	}
}

Balance::~Balance()
{
	//
}

GstColorBalanceChannel *Balance::Channel(const QString &name)
{
	return (channels.contains(name) ? channels[name] : nullptr);
}
