#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include "utils.h"
#include "gstreamer/gstreamer.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	Settings &settings = Settings::GetInstance();
	ui->video->createRenderer(settings.Video.Output);

	// Video Widget
	connect(ui->video, SIGNAL(positionChanged(UTime)), this, SLOT(positionUpdate(UTime)));
	connect(ui->video, SIGNAL(volumeChanged(double)), this, SLOT(volumeUpdate(double)));
	connect(ui->video, SIGNAL(stateChanged(PlayerInterface::State)), this, SLOT(stateUpdate(PlayerInterface::State)));
	connect(ui->video, SIGNAL(videoChanged()), this, SLOT(videoUpdate()));

	// Controls Widget
	connect(ui->controls, SIGNAL(open()), this, SLOT(open()));
	connect(ui->controls, SIGNAL(play()), ui->video, SLOT(play()));
	connect(ui->controls, SIGNAL(pause()), ui->video, SLOT(pause()));
	connect(ui->controls, SIGNAL(stop()), ui->video, SLOT(stop()));
	connect(ui->controls, SIGNAL(nextFrame()), ui->video, SLOT(nextFrame()));
	connect(ui->controls, SIGNAL(prevFrame()), ui->video, SLOT(prevFrame()));
	connect(ui->controls, SIGNAL(volumeChanged(double)), ui->video, SLOT(setVolume(double)));

	// Editor Widget
	connect(ui->editor, SIGNAL(hideWindow()), this, SLOT(toggleEditor()));
	connect(ui->editor, SIGNAL(jump(qint32)), ui->video, SLOT(seekFrame(qint32)));

	// hotkeys
	addHotkey(settings.KeysPlayer.PlayPause, ui->video, SLOT(toggle()));
	addHotkey(settings.KeysPlayer.Stop, ui->video, SLOT(stop()));
	addHotkey(settings.KeysPlayer.FullScreen, this, SLOT(fullScreen()));
	addHotkey(settings.KeysPlayer.Editor, this, SLOT(toggleEditor()));
	addHotkey(settings.KeysPlayer.Subtitles, ui->video, SLOT(toggleSubtitles()));
	addHotkey(settings.KeysPlayer.AspectRatio, ui->video, SLOT(toggleAspectRatio()));
	addHotkey(settings.KeysPlayer.SeekForward, this, SLOT(seekForward()));
	addHotkey(settings.KeysPlayer.SeekBackward, this, SLOT(seekBackward()));
	addHotkey(settings.KeysPlayer.VolumeUp, ui->video, SLOT(volumeUp()));
	addHotkey(settings.KeysPlayer.VolumeDown, ui->video, SLOT(volumeDown()));
	addHotkey(settings.KeysPlayer.FrameJump, this, SLOT(frameJump()));
	addHotkey(settings.KeysPlayer.TimeJump, this, SLOT(timeJump()));
	addHotkey(settings.KeysPlayer.NextFrame, ui->video, SLOT(nextFrame()));
	addHotkey(settings.KeysPlayer.PrevFrame, ui->video, SLOT(prevFrame()));
	addHotkey(settings.KeysPlayer.Time, ui->video, SLOT(toggleTime()));
	addHotkey(settings.KeysPlayer.HueUp, ui->video, SLOT(hueUp()));
	addHotkey(settings.KeysPlayer.HueDown, ui->video, SLOT(hueDown()));
	addHotkey(settings.KeysPlayer.SaturationUp, ui->video, SLOT(saturationUp()));
	addHotkey(settings.KeysPlayer.SaturationDown, ui->video, SLOT(saturationDown()));
	addHotkey(settings.KeysPlayer.BrightnessUp, ui->video, SLOT(brightnessUp()));
	addHotkey(settings.KeysPlayer.BrightnessDown, ui->video, SLOT(brightnessDown()));
	addHotkey(settings.KeysPlayer.ContrastUp, ui->video, SLOT(contrastUp()));
	addHotkey(settings.KeysPlayer.ContrastDown, ui->video, SLOT(contrastDown()));

	{
		QMap<const char *, const QObject *> slot;
		slot.insert(SLOT(toggleMute()), ui->video);
		slot.insert(SLOT(toggleMute()), ui->controls);
		addHotkey(settings.KeysPlayer.Mute, slot);
	}

	// jumper dialog
	jumper = new JumpDialog(this);

	// on start
	ui->controls->onStateStopped();
	positionUpdate(UTime());
}

MainWindow::~MainWindow()
{
	delete ui;
}

QString MainWindow::GetVersionInfo()
{
	QString info;
	return info.sprintf("Version: %d.%d.%d [Codename \"%s\"]", _MAJOR_, _MINOR_, _REVISION_, _CODENAME_);
}

void MainWindow::openFile()
{
	Settings &settings = Settings::GetInstance();
	if (settings.Gui.RememberDir)
		movieDir = settings.Gui.LastDir;
	else
		movieDir = settings.Gui.VideoDir;

	QString file = QFileDialog::getOpenFileName(this, tr("Open Video File"), movieDir);
	if (!file.isEmpty())
	{
		settings.Gui.LastDir =  QFileInfo(file).path();
		playFile(file);
	}
}

void MainWindow::playFile(const QString &url)
{
	QString local = QUrl(url).toLocalFile();
	QString subfilename = local.left(local.lastIndexOf('.')) + ".txt"; // .sub .srt
	QFile film(local);
	QFile subtitles(subfilename);
	if (film.exists())
	{
		ui->video->stop();
		ui->video->setVideo(url);
		Settings &settings = Settings::GetInstance();
		if (subtitles.exists())
		{
			ui->video->setSubtitles(subfilename);
			ui->video->setSubtitlesFont(settings.Subtitles.Font, settings.Subtitles.Encoding);
			ui->editor->loadSubtitles(subfilename);
		}

		if (settings.Gui.RememberPosition)
			ui->video->play(settings.getPosition(url));
		else
			ui->video->play();

	}
}

void MainWindow::playUrl(const QString &url)
{
	QUrl uri(url);
	if (uri.isValid())
	{
		ui->video->stop();
		ui->video->setVideo(uri.toString());
		ui->video->play();
	}
}

void MainWindow::startPlaying(const QString &url)
{
	this->url = url;
	play();
}

void MainWindow::addHotkey(const QKeySequence &key, QMap<const char *, const QObject *> slot)
{
	QShortcut *shortcut = new QShortcut(key, this);
	hotkeys.push_back(shortcut);
	foreach (const char *i, slot.keys())
	{
		connect(shortcut, SIGNAL(activated()), slot[i], i);
	}
}

void MainWindow::addHotkey(const QKeySequence &key, const QObject *obj, const char *slot)
{
	QShortcut *shortcut = new QShortcut(key, this);
	hotkeys.push_back(shortcut);
	connect(shortcut, SIGNAL(activated()), obj, slot);
}

void MainWindow::changeEvent(QEvent *event)
{
	if (event->type() == QEvent::WindowStateChange)
	{
		qDebug() << (windowState() & Qt::WindowFullScreen);
		// if full screen do sth
	}
	QMainWindow::changeEvent(event);
}

void MainWindow::positionUpdate(UTime position)
{
	QString message;
	message = position.Time.toString("hh:mm:ss.zzz") + "/" + length.Time.toString("hh:mm:ss.zzz");
	message += "\tFrame: " + QString::number(position.Frame);
	message += "\tName: " + videoPath;

	ui->statusBar->showMessage(message);
}

void MainWindow::volumeUpdate(double volume)
{
	ui->controls->setVolume(volume);
}

void MainWindow::stateUpdate(PlayerInterface::State state)
{
	switch (state)
	{
	case PlayerInterface::STOPPED:
		ui->video->update();
		ui->controls->onStateStopped();
		break;
	case PlayerInterface::PAUSED:
		ui->controls->onStatePaused();
		break;
	case PlayerInterface::PLAYING:
		ui->controls->onStatePlaying();
		break;
	default:
		break;
	}
}

void MainWindow::open()
{
	openFile();
}

void MainWindow::play()
{
	play(url);
}

void MainWindow::play(QString video)
{
	QUrl url(video);
	if (url.isValid())
	{
		if (video.indexOf("://") < 0)
		{
			playFile(QUrl::fromLocalFile(video).toString());
		}
		else
		{
			playUrl(video);
		}
	}
}

void MainWindow::toggleEditor()
{
	if (ui->editor->isVisible())
	{
		ui->editor->hide();
		ui->centralWidget->setFocus();
	}
	else
	{
		ui->editor->show();
		ui->editor->setFocus();
	}
}

void MainWindow::fullScreen()
{
	setWindowState(windowState() ^ Qt::WindowFullScreen);

	Settings &settings = Settings::GetInstance();
	static bool controlBarState = !settings.Gui.ControlBar;
	static bool statusBarState = !settings.Gui.StatusBar;
	static bool editorState = !settings.Gui.Editor;

	if (isFullScreen())
	{
		controlBarState = ui->controls->isHidden();
		statusBarState = ui->statusBar->isHidden();
		editorState = ui->editor->isHidden();
		ui->controls->hide();
		ui->statusBar->hide();
		ui->editor->hide();
	}
	else
	{
		if (!controlBarState)
			ui->controls->show();
		if (!statusBarState)
			ui->statusBar->show();
		if (!editorState)
			ui->editor->show();
	}
}

void MainWindow::frameJump()
{
	qint32 position;
	jumper->setType(FrameJump);
	if (jumper->exec())
	{
		position = jumper->getInput().toInt();
		ui->video->gotoFrame(position, true);
	}
}

void MainWindow::timeJump()
{
	QTime position;
	jumper->setType(TimeJump);
	if (jumper->exec())
	{
		position = QTime::fromString(jumper->getInput(), "h:m:s");
		ui->video->gotoTime(position, true);
	}
}

void MainWindow::seekBackward()
{
	Settings &settings = Settings::GetInstance();
	ui->video->seekBackward(settings.Video.SeekShort);
}

void MainWindow::seekForward()
{
	Settings &settings = Settings::GetInstance();
	ui->video->seekForward(settings.Video.SeekShort);
}

void MainWindow::videoUpdate()
{
	videoPath = ui->video->getVideoPath();
	length = ui->video->getDuration();
}
