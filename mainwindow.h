#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QShortcut>
#include <QUrl>
#include "player.h"
#include "metadata.h"
#include "utime.h"
#include "controls.h"
#include "settings.h"
#include "jumpdialog.h"
#include "version.h"

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
	void playFile(const QString &url);
	void playUrl(const QString &url);
	void startPlaying(const QString &url);
	void addHotkey(const QKeySequence &key, const QObject *obj, const char *slot);
	void addHotkey(const QKeySequence &key, QMap<const char *, const QObject *> slot);

	static QString GetVersionInfo();

protected:
	void changeEvent(QEvent *event);

private:
	Ui::MainWindow *ui;
	QString movieDir;
	QVector<QShortcut *> hotkeys;
	JumpDialog *jumper;
	QString url;
	QString videoPath;
	UTime length;

public slots:
	void positionUpdate(UTime);
	void volumeUpdate(double);
	void stateUpdate(PlayerInterface::State);
	void open();
	void play();
	void play(QString);
	void toggleEditor();
	void fullScreen();
	void frameJump();
	void timeJump();
	void seekBackward();
	void seekForward();
	void videoUpdate();
};

#endif // MAINWINDOW_H
