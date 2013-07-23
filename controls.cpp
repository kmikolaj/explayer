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
