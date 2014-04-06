#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "utils.h"
#include <QFileDialog>
#include "gstreamer/gstreamer.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	settings = Settings::GetSettings(this);
	player = new Gstreamer(ui->video);

	// Video Widget
	connect(player, SIGNAL(positionChanged()), this, SLOT(positionUpdate()));
	connect(player, SIGNAL(volumeChanged(double)), this, SLOT(volumeUpdate(double)));
	connect(player, SIGNAL(stateChanged(PlayerInterface::State)), this, SLOT(stateUpdate(PlayerInterface::State)));

	// Controls Widget
	connect(ui->controls, SIGNAL(play()), player, SLOT(play()));
	connect(ui->controls, SIGNAL(pause()), player, SLOT(pause()));
	connect(ui->controls, SIGNAL(stop()), player, SLOT(stop()));
	connect(ui->controls, SIGNAL(open()), this, SLOT(open()));
	connect(ui->controls, SIGNAL(nextFrame()), this, SLOT(nextFrame()));
	connect(ui->controls, SIGNAL(prevFrame()), this, SLOT(prevFrame()));
	connect(ui->controls, SIGNAL(volumeChanged(double)), player, SLOT(setVolume(double)));

	// Editor Widget
	connect(ui->editor, SIGNAL(hideWindow()), this, SLOT(toggleeditor()));
	connect(ui->editor, SIGNAL(jump(qint32)), this, SLOT(seekFrame(qint32)));

	// hotkeys
	addHotkey(settings->KeysPlayer.PlayPause, player, SLOT(toggle()));
	addHotkey(settings->KeysPlayer.Stop, player, SLOT(stop()));
	addHotkey(settings->KeysPlayer.FullScreen, this, SLOT(fullScreen()));
	addHotkey(settings->KeysPlayer.Editor, this, SLOT(toggleeditor()));
	addHotkey(settings->KeysPlayer.Subtitles, player, SLOT(togglesubtitles()));
	addHotkey(settings->KeysPlayer.AspectRatio, player, SLOT(forceaspectratio()));
	addHotkey(settings->KeysPlayer.SeekForward, this, SLOT(seekForward()));
	addHotkey(settings->KeysPlayer.SeekBackward, this, SLOT(seekBackward()));
	addHotkey(settings->KeysPlayer.VolumeUp, this, SLOT(volumeUp()));
	addHotkey(settings->KeysPlayer.VolumeDown, this, SLOT(volumeDown()));
	addHotkey(settings->KeysPlayer.FrameJump, this, SLOT(frameJump()));
	addHotkey(settings->KeysPlayer.TimeJump, this, SLOT(timeJump()));
	addHotkey(settings->KeysPlayer.NextFrame, this, SLOT(nextFrame()));
	addHotkey(settings->KeysPlayer.PrevFrame, this, SLOT(prevFrame()));
	addHotkey(settings->KeysPlayer.Time, player, SLOT(toggletime()));
	addHotkey(settings->KeysPlayer.HueUp, this, SLOT(hueUp()));
	addHotkey(settings->KeysPlayer.HueDown, this, SLOT(hueDown()));
	addHotkey(settings->KeysPlayer.SaturationUp, this, SLOT(saturationUp()));
	addHotkey(settings->KeysPlayer.SaturationDown, this, SLOT(saturationDown()));
	addHotkey(settings->KeysPlayer.BrightnessUp, this, SLOT(brightnessUp()));
	addHotkey(settings->KeysPlayer.BrightnessDown, this, SLOT(brightnessDown()));
	addHotkey(settings->KeysPlayer.ContrastUp, this, SLOT(contrastUp()));
	addHotkey(settings->KeysPlayer.ContrastDown, this, SLOT(contrastDown()));

	{
		QMap<const char *, const QObject *> slot;
		slot.insert(SLOT(mute()), player);
		slot.insert(SLOT(toggleMute()), ui->controls);
		addHotkey(settings->KeysPlayer.Mute, slot);
	}

	// jumper dialog
	jumper = new JumpDialog(this);

	// opener dialog
	// opener = new OpenDialog(this);

	// on start
	ui->controls->onStateStopped();
	positionUpdate();
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
	if (settings->Gui.RememberDir)
		movieDir = settings->Gui.LastDir;
	else
		movieDir = settings->Gui.VideoDir;
	QString file = QFileDialog::getOpenFileName(this, tr("Open Video File"), movieDir);
	if (!file.isEmpty())
	{
		settings->Gui.LastDir =  QFileInfo(file).path();
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
		player->stop();
		player->setVideo(url);
		if (subtitles.exists())
		{
			player->setSubtitles(subfilename);
			player->setFont(settings->Subtitles.Font, settings->Subtitles.Encoding);
			ui->editor->loadSubtitles(subfilename);
		}

		if (settings->Gui.RememberPosition)
			player->play(settings->getPosition(url));
		else
			player->play();
	}
}

void MainWindow::playUrl(const QString &url)
{
	QUrl uri(url);
	if (uri.isValid())
	{
		player->stop();
		player->setVideo(uri.toString());
		player->play();
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
	foreach(const char *i, slot.keys())
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

void MainWindow::seek(int seconds)
{
	UTime newPos = player->position();
	newPos.moveNsec(seconds * GST_SECOND);
	player->setPosition(newPos.Time);
	updateStatus(newPos, player->duration());
}

void MainWindow::gotoFrame(qint32 frame, bool pause)
{
	if (player->canSeek())
	{
		if (player->duration().Time > QTime(0, 0))
		{
			player->setPosition(frame);
			updateStatus(UTime(frame), player->duration());
			if (pause)
				player->pause();
		}
	}
}

void MainWindow::gotoTime(const QTime &time, bool pause)
{
	if (player->canSeek())
	{
		if (player->duration().Time > QTime(0, 0))
		{
			player->setPosition(time);
			updateStatus(UTime(time), player->duration());
			if (pause)
				player->pause();
		}
	}
}

void MainWindow::updateStatus(const UTime &position, const UTime &length)
{
	QString message;
	message = position.Time.toString("hh:mm:ss.zzz") + "/" + length.Time.toString("hh:mm:ss.zzz");
	message += "\tFrame: " + QString::number(position.Frame);
	message += "\tName: " + player->getVideoPath();

	ui->statusBar->showMessage(message);
}

void MainWindow::positionUpdate()
{
	UTime pos, len;

	if (player->canSeek())
	{
		pos = player->position();
		len = player->duration();
	}
	updateStatus(pos, len);
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
		player->update();
		ui->controls->onStateStopped();
		positionUpdate();
		break;
	case PlayerInterface::PAUSED:
		ui->controls->onStatePaused();
		positionUpdate();
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

void MainWindow::toggleeditor()
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
// FIX
void MainWindow::fullScreen()
{
	setWindowState(windowState() ^ Qt::WindowFullScreen);

//	static bool a = !settings->Gui.ControlBar;
	static bool b = !settings->Gui.StatusBar;
//	static bool c = !settings->Gui.Editor;

	if (isFullScreen())
	{
//		a = ui->controls->isHidden();
		b = ui->statusBar->isHidden();
//		c = ui->editor->isHidden();
//		ui->controls->hide();
		ui->statusBar->hide();
//		ui->editor->hide();
	}
	else
	{
//		if (!a) ui->controls->show();
		if (!b) ui->statusBar->show();
//		if (!c) ui->editor->show();
	}
}

void MainWindow::seekForward()
{
	if (player->canSeek())
	{
		seek(settings->Video.SeekShort);
	}
}

void MainWindow::seekBackward()
{
	if (player->canSeek())
	{
		seek(-1 * settings->Video.SeekShort);
	}
}

void MainWindow::seekFrame(qint32 frame)
{
	if (player->canSeek())
	{
		gotoFrame(frame, false);
	}
}

void MainWindow::nextFrame()
{
	if (player->canSeek())
	{
		gotoFrame(player->position().Frame + 1, true);
	}
}

void MainWindow::prevFrame()
{
	if (player->canSeek())
	{
		gotoFrame(player->position().Frame - 1, true);
	}
}

void MainWindow::volumeUp()
{
	double value = player->volume();
	player->setVolume(value + 0.05);
}

void MainWindow::volumeDown()
{
	double value = player->volume();
	player->setVolume(value - 0.05);
}

void MainWindow::hueUp()
{
	double value = player->hue();
	player->setHue(value + 0.1);
}

void MainWindow::hueDown()
{
	double value = player->hue();
	player->setHue(value - 0.1);
}

void MainWindow::saturationUp()
{
	double value = player->saturation();
	player->setSaturation(value + 0.1);
}

void MainWindow::saturationDown()
{
	double value = player->saturation();
	player->setSaturation(value - 0.1);
}

void MainWindow::brightnessUp()
{
	double value = player->brightness();
	player->setBrightness(value + 0.1);
}

void MainWindow::brightnessDown()
{
	double value = player->brightness();
	player->setBrightness(value - 0.1);
}

void MainWindow::contrastUp()
{
	double value = player->contrast();
	player->setContrast(value + 0.1);
}

void MainWindow::contrastDown()
{
	double value = player->contrast();
	player->setContrast(value - 0.1);
}

void MainWindow::frameJump()
{
	qint32 position;
	jumper->setType(FrameJump);
	if (jumper->exec())
	{
		position = jumper->getInput().toInt();
		gotoFrame(position, true);
	}
}

void MainWindow::timeJump()
{
	QTime position;
	jumper->setType(TimeJump);
	if (jumper->exec())
	{
		position = QTime::fromString(jumper->getInput(), "h:m:s");
		gotoTime(position, true);
	}
}
