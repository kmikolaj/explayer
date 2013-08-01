#include "mainwindow.h"
#include <QApplication>
#include <QGst/Init>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QGst::init(&argc, &argv);
	MainWindow w;
	w.show();

	if (argc > 1)
	{
		w.playFile(argv[1]);
	}

	return a.exec();
}
