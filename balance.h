#ifndef BALANCE_H
#define BALANCE_H

#include <gst/video/colorbalance.h>
#include <QObject>
#include <QMap>

class Balance : public QObject
{
	Q_OBJECT

public:
	Balance(GstElement *pipeline, QObject *parent=0);
	~Balance();
	GstColorBalanceChannel *Channel(const QString &);
	inline GstColorBalance *Get()
	{
		return balance;
	}

private:
	QMap<QString, GstColorBalanceChannel*> channels;
	GstColorBalance *balance;
};

#endif // BALANCE_H
