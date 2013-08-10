#include "jumpdialog.h"
#include "ui_jumpdialog.h"

JumpDialog::JumpDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::JumpDialog)
{
	ui->setupUi(this);
}

JumpDialog::~JumpDialog()
{
	delete ui;
}

void JumpDialog::setType(PositionJump position)
{
	QWidget *parent = qobject_cast<QWidget *>(this->parent());
	int left = (parent->width() - this->width()) / 2;
	int top = (parent->height() - this->height()) / 2;
	move(parent->geometry().left() + left, parent->geometry().top() + top);
	if ((type = position) == TimeJump)
		ui->label->setText(tr("Time:"));
	else
		ui->label->setText(tr("Frame:"));
	ui->position->clear();
}

QString JumpDialog::getInput()
{
	return ui->position->text();
}
