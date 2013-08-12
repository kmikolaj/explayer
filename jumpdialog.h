#ifndef JUMPDIALOG_H
#define JUMPDIALOG_H

#include <QDialog>

enum PositionJump
{
    TimeJump,
    FrameJump
};

namespace Ui
{
class JumpDialog;
}

class JumpDialog : public QDialog
{
	Q_OBJECT

public:
	explicit JumpDialog(QWidget *parent = 0);
	~JumpDialog();

	void setType(PositionJump position = FrameJump);
	QString getInput();

private:
	Ui::JumpDialog *ui;
	PositionJump type;
};

#endif // JUMPDIALOG_H
