#include "player.h"

PlayerInterface::PlayerInterface(QWidget *parent) :
	QWidget(parent), surface(parent), metadata(nullptr)
{
}

PlayerInterface::~PlayerInterface()
{
	delete metadata;
}

void PlayerInterface::disableDPMS()
{
	dpms.Disable();
}

void PlayerInterface::enableDPMS()
{
	dpms.Enable();
}

Metadata *PlayerInterface::getMetadata() const
{
	return metadata;
}

void PlayerInterface::setMetadata(Metadata *value)
{
	metadata = value;
}

QString PlayerInterface::getVideoPath() const
{
	return videoPath;
}


