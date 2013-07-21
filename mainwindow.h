#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QUrl>
#include "player.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT
	
public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	void openFile();
	void playFile(const QString &filename);
	void playUrl(const QUrl &url);

private:
	Ui::MainWindow *ui;
	QString movieDir;

private slots:
	void gotoFrame(qint64 frame, bool pause=false);
	void gotoTime(qint64 time, bool pause=false);
	void positionChanged();
	void setState();
	void on_pushButton_clicked();

	void on_pushButton_2_clicked();
};

#endif // MAINWINDOW_H
