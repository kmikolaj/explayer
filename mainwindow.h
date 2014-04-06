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
	Settings *settings;
	QString movieDir;
	QVector<QShortcut *> hotkeys;
	JumpDialog *jumper;
	QString url;
	PlayerInterface *player;

	void seek(int seconds);
	void gotoFrame(qint32 frame, bool pause = false);
	void gotoTime(const QTime &time, bool pause = false);
	void updateStatus(const UTime &position, const UTime &length);

public slots:
	void positionUpdate();
	void volumeUpdate(double);
	void stateUpdate(PlayerInterface::State);
	void open();
	void play();
	void play(QString);
	void toggleeditor();
	void fullScreen();
	void seekForward();
	void seekBackward();
	void seekFrame(qint32);
	void nextFrame();
	void prevFrame();
	void volumeUp();
	void volumeDown();
	void hueUp();
	void hueDown();
	void saturationUp();
	void saturationDown();
	void brightnessUp();
	void brightnessDown();
	void contrastUp();
	void contrastDown();
	void frameJump();
	void timeJump();
};

#endif // MAINWINDOW_H
