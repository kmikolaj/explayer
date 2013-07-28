#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QShortcut>
#include <QUrl>
#include "player.h"
#include "controls.h"
#include "settings.h"

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
	void addHotkey(const QString &key, const QObject *obj, const char *slot);

private:
	Ui::MainWindow *ui;
	Settings *settings;
	QString movieDir;
	QVector<QShortcut *> hotkeys;
	bool editor;

private slots:
	void gotoFrame(qint64 frame, bool pause=false);
	void gotoTime(qint64 time, bool pause=false);
	void positionChanged();
	void setState();
	void open();
	void toggleeditor();
	void fullScreen();
	void on_editor_cursorPositionChanged();
};

#endif // MAINWINDOW_H
