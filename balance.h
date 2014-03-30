#ifndef BALANCE_H
#define BALANCE_H

#if GST_VERSION_MAJOR == 1
#include <gst/video/colorbalance.h>
#else
#include <gst/interfaces/colorbalance.h>
#endif
#include <QObject>
#include <QMap>

class Balance : public QObject
{
	Q_OBJECT
public:
	Balance(GstElement *pipeline, QObject *parent=0);
	~Balance();
	GstColorBalanceChannel *Channel(const QString &);
	operator GstColorBalance*() { return balance; }

private:
	QMap<QString, GstColorBalanceChannel*> channels;
	GstColorBalance *balance;
};

#endif // BALANCE_H
