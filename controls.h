#ifndef CONTROLS_H
#define CONTROLS_H

#include <QWidget>

namespace Ui {
class Controls;
}

class Controls : public QWidget
{
	Q_OBJECT
	
public:
	explicit Controls(QWidget *parent = 0);
	~Controls();
	
private:
	Ui::Controls *ui;
};

#endif // CONTROLS_H
