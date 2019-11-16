#include "ui/mainwindow.h"
#include <QApplication>
#include <iostream>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	// create main window
	MainWindow w;
	w.show();

	std::cout << qPrintable(MainWindow::GetVersionInfo()) << std::endl;

	if (argc > 1)
	{
		w.startPlaying(argv[1]);
	}

	return a.exec();
}
