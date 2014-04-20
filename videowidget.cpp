#include "videowidget.h"
#include "gstreamer/gstreamer.h"
// FIX if player
VideoWidget::VideoWidget(QWidget *parent) :
    QWidget(parent), player(nullptr)
{
	settings = Settings::GetSettings(this);
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
		player = nullptr;
}

bool VideoWidget::hasRenderer()
{
	return player;
}

void VideoWidget::paintEvent(QPaintEvent *event)
{
	QPainter p(this);
	p.fillRect(event->rect(), Qt::black);
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
	if (player->canSeek())
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
	if (player->canSeek())
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
	if (player->canSeek())
	{
		gotoFrame(player->position().Frame + 1, true);
	}
}

void VideoWidget::prevFrame()
{
	if (player->canSeek())
	{
		gotoFrame(player->position().Frame - 1, true);
	}
}

void VideoWidget::seekForward()
{
	if (player->canSeek())
	{
		seek(settings->Video.SeekShort);
	}
}

void VideoWidget::seekBackward()
{
	if (player->canSeek())
	{
		seek(-1 * settings->Video.SeekShort);
	}
}

void VideoWidget::seekFrame(qint32 frame)
{
	if (player->canSeek())
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
