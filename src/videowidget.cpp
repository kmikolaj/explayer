#include "videowidget.h"
#include "gstreamer/gstreamer.h"
#include <QDebug>
// FIX if player
VideoWidget::VideoWidget(QWidget *parent) :
	QWidget(parent), player(nullptr)
{
	QPalette palette = QWidget::palette();
	palette.setColor(QPalette::Background, Qt::black);
	setPalette(palette);
}

void VideoWidget::createRenderer(const QString &output)
{
	delete player;
	if (output == "gstreamer")
	{
		player = new Gstreamer(this);
		connect(player, SIGNAL(videoChanged()), this, SIGNAL(videoChanged()));
		connect(player, SIGNAL(volumeChanged(double)), this, SIGNAL(volumeChanged(double)));
		connect(player, SIGNAL(stateChanged(PlayerInterface::State)), this, SIGNAL(stateChanged(PlayerInterface::State)));
		connect(player, SIGNAL(positionChanged()), this, SLOT(onPositionChanged()));
	}
	else
	{
		player = nullptr;
	}
}

bool VideoWidget::hasRenderer()
{
	return player;
}

void VideoWidget::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event)
	QPainter painter(this);
	painter.fillRect(rect(), palette().background());
}

void VideoWidget::resizeEvent(QResizeEvent *event)
{
	player->expose();
	QWidget::resizeEvent(event);
}

void VideoWidget::showEvent(QShowEvent *event)
{
	setAttribute(Qt::WA_NoSystemBackground, true);
	QWidget::showEvent(event);
}

void VideoWidget::onPositionChanged()
{
	emit positionChanged(player->position());
}

void VideoWidget::play(const QTime &time)
{
	if (time == QTime())
		player->play();
	else
		player->play(time);
}

void VideoWidget::pause()
{
	player->pause();
}

void VideoWidget::toggle()
{
	player->toggle();
}

void VideoWidget::stop()
{
	player->stop();
}

void VideoWidget::seek(int seconds)
{
	UTime newPos = player->position();
	newPos.moveNsec(seconds * GST_SECOND);
	player->setPosition(newPos.Time);
	emit positionChanged(newPos);
}

void VideoWidget::gotoFrame(qint32 frame, bool pause)
{
	if (player->isSeekable())
	{
		if (player->duration().Time > QTime(0, 0))
		{
			player->setPosition(frame);
			emit positionChanged(UTime(frame));
			if (pause)
				player->pause();
		}
	}
}

void VideoWidget::gotoTime(const QTime &time, bool pause)
{
	if (player->isSeekable())
	{
		if (player->duration().Time > QTime(0, 0))
		{
			player->setPosition(time);
			emit positionChanged(UTime(time));
			if (pause)
				player->pause();
		}
	}
}

QString VideoWidget::getVideoPath()
{
	return player->getVideoPath();
}

UTime VideoWidget::getDuration()
{
	return player->getMetadata()->getDuration();
}

void VideoWidget::setVideo(const QString &video)
{
	player->setVideo(video);
}

void VideoWidget::setSubtitles(const QString &subs)
{
	player->setSubtitles(subs);
}

void VideoWidget::setSubtitlesFont(const QString &font, const QString &encoding)
{
	player->setFont(font, encoding);
}

void VideoWidget::nextFrame()
{
	if (player->isSeekable())
	{
		gotoFrame(player->position().Frame + 1, true);
	}
}

void VideoWidget::prevFrame()
{
	if (player->isSeekable())
	{
		gotoFrame(player->position().Frame - 1, true);
	}
}

void VideoWidget::seekForward(int range)
{
	if (player->isSeekable())
	{
		seek(range);
	}
}

void VideoWidget::seekBackward(int range)
{
	if (player->isSeekable())
	{
		seek(-range);
	}
}

void VideoWidget::seekFrame(qint32 frame)
{
	if (player->isSeekable())
	{
		gotoFrame(frame, false);
	}
}

void VideoWidget::setVolume(double volume)
{
	player->setVolume(volume);
}

void VideoWidget::volumeUp()
{
	double value = player->volume();
	player->setVolume(value + 0.05);
}

void VideoWidget::volumeDown()
{
	double value = player->volume();
	player->setVolume(value - 0.05);
}

void VideoWidget::hueUp()
{
	double value = player->hue();
	player->setHue(value + 0.1);
}

void VideoWidget::hueDown()
{
	double value = player->hue();
	player->setHue(value - 0.1);
}

void VideoWidget::saturationUp()
{
	double value = player->saturation();
	player->setSaturation(value + 0.1);
}

void VideoWidget::saturationDown()
{
	double value = player->saturation();
	player->setSaturation(value - 0.1);
}

void VideoWidget::brightnessUp()
{
	double value = player->brightness();
	player->setBrightness(value + 0.1);
}

void VideoWidget::brightnessDown()
{
	double value = player->brightness();
	player->setBrightness(value - 0.1);
}

void VideoWidget::contrastUp()
{
	double value = player->contrast();
	player->setContrast(value + 0.1);
}

void VideoWidget::contrastDown()
{
	double value = player->contrast();
	player->setContrast(value - 0.1);
}

void VideoWidget::toggleSubtitles()
{
	player->togglesubtitles();
}

void VideoWidget::toggleAspectRatio()
{
	player->forceaspectratio();
}

void VideoWidget::toggleTime()
{
	player->toggletime();
}

void VideoWidget::toggleMute()
{
	player->mute();
}
