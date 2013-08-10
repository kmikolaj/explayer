#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QShortcut>
#include <QUrl>
#include "player.h"
#include "controls.h"
#include "settings.h"
#include "jumpdialog.h"

namespace Ui
{
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
	void addHotkey(const QKeySequence &key, const QObject *obj, const char *slot);

private:
	Ui::MainWindow *ui;
	Settings *settings;
	QString movieDir;
	QVector<QShortcut *> hotkeys;
	bool editor;
	JumpDialog *jumper;

	void seek(int seconds);
	void gotoFrame(qint32 frame, bool pause = false);
	void gotoTime(const QTime &time, bool pause = false);
	void updateStatus(const GstTime &position, const GstTime &length);

private slots:
	void positionUpdate();
	void stateUpdate();
	void open();
	void toggleeditor();
	void fullScreen();
	void seekForward();
	void seekBackward();
	void nextFrame();
	void prevFrame();
	void volumeUp();
	void volumeDown();
	void frameJump();
	void timeJump();
};

#endif // MAINWINDOW_H
