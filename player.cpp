#include "player.h"

Player::Player(QWidget *parent) :
	QWidget(parent)
{
	output = new Gstreamer(this);
	output->setHardwareAcceleration(false);
}

void Player::setVideo(const QString &uri)
{
	output->setVideo(uri);
}

void Player::setSubtitles(const QString &sub, const QString &font, const QString &enc)
{
	output->setSubtitles(sub);
}

void Player::disableDPMS()
{
	//
}

void Player::enableDPMS()
{
	//
}

void Player::paintEvent(QPaintEvent *event)
{
	output->setVideoArea(rect());
	output->expose();
}

void Player::play()
{
	output->play();
}

void Player::play(QTime time)
{
	output->play();
}

void Player::pause()
{
	output->pause();
}

void Player::toggle()
{
	output->toggle();
}

void Player::stop()
{
	output->stop();
}

void Player::setBrightness(double brightness)
{
	output->setBrightness(brightness);
}

void Player::forceaspectratio()
{
	output->forceaspectratio();
}

void Player::togglesubtitles()
{
	output->togglesubtitles();
}

void Player::toggletime()
{
	output->toggletime();
}

void Player::mute()
{
	output->mute();
}
