#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QWidget>
#include <QResizeEvent>
#include <QPaintEvent>
#include <QPainter>
#include "player.h"

class VideoWidget : public QWidget
{
	Q_OBJECT
public:
	explicit VideoWidget(QWidget *parent = 0);

protected:
	void paintEvent(QPaintEvent *event);

private:
	PlayerInterface *player;

signals:

public slots:

};

#endif // VIDEOWIDGET_H
