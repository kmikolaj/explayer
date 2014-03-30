#include "player.h"

PlayerInterface::PlayerInterface(QWidget *parent) :
	QWidget(parent)
{
}

void PlayerInterface::disableDPMS()
{
	dpms.Disable();
}

void PlayerInterface::enableDPMS()
{
	dpms.Enable();
}
