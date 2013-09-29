#ifndef PLAYER_H
#define PLAYER_H

#include <QWidget>
#include "gstreamer.h"

class Player : public QWidget
{
	Q_OBJECT
public:
	explicit Player(QWidget *parent = 0);

private:
	Gstreamer output;

signals:

public slots:

};

#endif // PLAYER_H
