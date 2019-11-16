#include "controls.h"
#include "ui_controls.h"

Controls::Controls(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Controls), mute(false)
{
	ui->setupUi(this);
}

Controls::~Controls()
{
	delete ui;
}

void Controls::setVolume(int volume)
{
	ui->volumeSlider->setValue(volume);
}

void Controls::enable(ControlElement element)
{
	switch (element)
	{
	case Play:
		ui->playButton->setEnabled(true);
		break;
	case Pause:
		ui->pauseButton->setEnabled(true);
		break;
	case Stop:
		ui->stopButton->setEnabled(true);
		break;
	case PrevFrame:
		ui->prevFrameButton->setEnabled(true);
		break;
	case NextFrame:
		ui->nextFrameButton->setEnabled(true);
		break;
	case Open:
		ui->openButton->setEnabled(true);
		break;
	case Mute:
		ui->muteButton->setEnabled(true);
		break;
	case Volume:
		ui->volumeSlider->setEnabled(true);
		break;
	}
}

void Controls::disable(ControlElement element)
{
	switch (element)
	{
	case Play:
		ui->playButton->setEnabled(false);
		break;
	case Pause:
		ui->pauseButton->setEnabled(false);
		break;
	case Stop:
		ui->stopButton->setEnabled(false);
		break;
	case PrevFrame:
		ui->prevFrameButton->setEnabled(false);
		break;
	case NextFrame:
		ui->nextFrameButton->setEnabled(false);
		break;
	case Open:
		ui->openButton->setEnabled(false);
		break;
	case Mute:
		ui->muteButton->setEnabled(false);
		break;
	case Volume:
		ui->volumeSlider->setEnabled(false);
		break;
	}
}

void Controls::toggleMute()
{
	mute = !mute;
	if (mute)
	{
		ui->muteButton->setIcon(QIcon::fromTheme("audio-volume-muted"));
	}
	else
	{
		on_volumeSlider_valueChanged(ui->volumeSlider->value());
	}
}

void Controls::onStatePlaying()
{
	disable(Play);
	enable(Pause);
	enable(Stop);
	enable(NextFrame);
	enable(PrevFrame);
	enable(Volume);
}

void Controls::onStatePaused()
{
	enable(Play);
	disable(Pause);
}

void Controls::onStateStopped()
{
	enable(Play);
	disable(Pause);
	disable(Stop);
	disable(NextFrame);
	disable(PrevFrame);
	disable(Volume);
}

void Controls::on_playButton_clicked()
{
	emit play();
}

void Controls::on_pauseButton_clicked()
{
	emit pause();
}

void Controls::on_stopButton_clicked()
{
	emit stop();
}

void Controls::on_prevFrameButton_clicked()
{
	emit prevFrame();
}

void Controls::on_nextFrameButton_clicked()
{
	emit nextFrame();
}

void Controls::on_openButton_clicked()
{
	emit open();
}

void Controls::on_volumeSlider_sliderMoved(int position)
{
	emit volumeChanged(double(position) / 100.0);
}

void Controls::on_volumeSlider_valueChanged(int value)
{
	if (value < 1)
		ui->muteButton->setIcon(QIcon::fromTheme("audio-volume-muted"));
	else if (value < 34)
		ui->muteButton->setIcon(QIcon::fromTheme("audio-volume-low"));
	else if (value < 67)
		ui->muteButton->setIcon(QIcon::fromTheme("audio-volume-medium"));
	else
		ui->muteButton->setIcon(QIcon::fromTheme("audio-volume-high"));
}
