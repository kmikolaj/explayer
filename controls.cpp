#include "controls.h"
#include "ui_controls.h"

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
	emit prevframe();
}

void Controls::on_nextFrameButton_clicked()
{
	emit nextframe();
}

void Controls::on_openButton_clicked()
{
	emit open();
}
