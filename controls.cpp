#include "controls.h"
#include "ui_controls.h"
#include <QGst/StreamVolume>

Controls::Controls(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Controls)
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
	double newPos = QGst::StreamVolume::convert(QGst::StreamVolumeFormatLinear, QGst::StreamVolumeFormatCubic, position / 100.0);
	emit volumeChanged(newPos);
}
