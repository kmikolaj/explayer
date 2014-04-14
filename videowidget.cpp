#include "videowidget.h"

VideoWidget::VideoWidget(QWidget *parent) :
    QWidget(parent), player(nullptr)
{
}

void VideoWidget::paintEvent(QPaintEvent *event)
{
	QPainter p(this);
    p.fillRect(event->rect(), Qt::black);
}
