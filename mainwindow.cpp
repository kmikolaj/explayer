#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "utils.h"
#include <QFileDialog>
#include <QGst/StreamVolume>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
    ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	settings = Settings::GetSettings(this);
	ui->editor->setPlayer(ui->video);

	// Video Widget
	connect(ui->video, SIGNAL(positionChanged()), this, SLOT(positionUpdate()));
	connect(ui->video, SIGNAL(stateChanged()), this, SLOT(stateUpdate()));

	// Controls Widget
	connect(ui->controls, SIGNAL(play()), ui->video, SLOT(play()));
	connect(ui->controls, SIGNAL(pause()), ui->video, SLOT(pause()));
	connect(ui->controls, SIGNAL(stop()), ui->video, SLOT(stop()));
	connect(ui->controls, SIGNAL(open()), this, SLOT(open()));
	connect(ui->controls, SIGNAL(volumeChanged(double)), ui->video, SLOT(setVolume(double)));

	// Editor Widget
	connect(ui->editor, SIGNAL(hideWindow()), this, SLOT(toggleeditor()));
	connect(ui->editor, SIGNAL(jump(qint32)), this, SLOT(seekFrame(qint32)));

	// hotkeys
	addHotkey(settings->KeysPlayer.PlayPause, ui->video, SLOT(toggle()));
	addHotkey(settings->KeysPlayer.Stop, ui->video, SLOT(stop()));
	addHotkey(settings->KeysPlayer.FullScreen, this, SLOT(fullScreen()));
	addHotkey(settings->KeysPlayer.Editor, this, SLOT(toggleeditor()));
	addHotkey(settings->KeysPlayer.Subtitles, ui->video, SLOT(togglesubtitles()));
	addHotkey(settings->KeysPlayer.AspectRatio, ui->video, SLOT(forceaspectratio()));
	addHotkey(settings->KeysPlayer.SeekForward, this, SLOT(seekForward()));
	addHotkey(settings->KeysPlayer.SeekBackward, this, SLOT(seekBackward()));
	addHotkey(settings->KeysPlayer.VolumeUp, this, SLOT(volumeUp()));
	addHotkey(settings->KeysPlayer.VolumeDown, this, SLOT(volumeDown()));
	addHotkey(settings->KeysPlayer.Mute, ui->video, SLOT(mute()));
	addHotkey(settings->KeysPlayer.FrameJump, this, SLOT(frameJump()));
	addHotkey(settings->KeysPlayer.TimeJump, this, SLOT(timeJump()));
	addHotkey(settings->KeysPlayer.NextFrame, this, SLOT(nextFrame()));
	addHotkey(settings->KeysPlayer.PrevFrame, this, SLOT(prevFrame()));
	addHotkey(settings->KeysPlayer.Time, ui->video, SLOT(toggletime()));
	addHotkey(settings->KeysPlayer.HueUp, this, SLOT(hueUp()));
	addHotkey(settings->KeysPlayer.HueDown, this, SLOT(hueDown()));
	addHotkey(settings->KeysPlayer.SaturationUp, this, SLOT(saturationUp()));
	addHotkey(settings->KeysPlayer.SaturationDown, this, SLOT(saturationDown()));
	addHotkey(settings->KeysPlayer.BrightnessUp, this, SLOT(brightnessUp()));
	addHotkey(settings->KeysPlayer.BrightnessDown, this, SLOT(brightnessDown()));
	addHotkey(settings->KeysPlayer.ContrastUp, this, SLOT(contrastUp()));
	addHotkey(settings->KeysPlayer.ContrastDown, this, SLOT(contrastDown()));

	// jumper dialog
	jumper = new JumpDialog(this);

	// on start
	positionUpdate();
	if (!(editor = settings->Gui.Editor))
		ui->editor->hide();
}

MainWindow::~MainWindow()
{
	delete ui;
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

void MainWindow::playFile(const QString &filename)
{
	QString subfilename = filename.left(filename.lastIndexOf('.')) + ".txt";
	QFile film(filename);
	QFile subtitles(subfilename);
	if (film.exists())
	{
		ui->video->stop();
		ui->video->setUri(filename);
		if (subtitles.exists())
		{
			ui->video->setSubtitles(subfilename, settings->Subtitles.Font, settings->Subtitles.Encoding);
			ui->editor->loadSubtitles(subfilename);
		}
		ui->video->play();
	}
}

void MainWindow::startPlaying(const QString &url)
{
	// detekcja czy lokalny
	this->url = url;
	QTimer::singleShot(50, this, SLOT(play()));
}

void MainWindow::playUrl(const QUrl &url)
{
	ui->video->stop();
	ui->video->setUri(url.toString());
	ui->video->play();
}

void MainWindow::addHotkey(const QString &key, const QObject *obj, const char *slot)
{
	QShortcut *shortcut = new QShortcut(QKeySequence(key), this);
	hotkeys.append(shortcut);
	connect(shortcut, SIGNAL(activated()), obj, slot);
}

void MainWindow::addHotkey(const QKeySequence &key, const QObject *obj, const char *slot)
{
	QShortcut *shortcut = new QShortcut(key, this);
	hotkeys.append(shortcut);
	connect(shortcut, SIGNAL(activated()), obj, slot);
}

void MainWindow::seek(int seconds)
{
	GstTime newPos = ui->video->position();
	newPos.moveMsec(1000 * seconds);
	if (newPos.Time >= QTime(0, 0) && newPos.Time <= ui->video->length().Time)
	{
		if (ui->video->metadata().getTags().containerFormat() == "ASF")
			ui->video->setPosition(newPos, Accurate);
		else
			ui->video->setPosition(newPos);
		updateStatus(newPos, ui->video->length());
	}
}

void MainWindow::gotoFrame(qint32 frame, bool pause)
{
	if (ui->video->state() != QGst::StateNull)
	{
		GstTime newPos = GstTime(frame);
		if (ui->video->length().Time > QTime(0, 0))
		{
			ui->video->setPosition(newPos, SeekFlag(Accurate | Skip));
			updateStatus(newPos, ui->video->length());
			if (pause)
				ui->video->pause();
		}
	}
}

void MainWindow::gotoTime(const QTime &time, bool pause)
{
	if (ui->video->state() != QGst::StateNull)
	{
		if (ui->video->length().Time > QTime(0, 0))
		{
			ui->video->setPosition(GstTime(time));
			if (pause)
				ui->video->pause();
		}
	}
}

void MainWindow::updateStatus(const GstTime &position, const GstTime &length)
{
	QString message;

	message = position.Time.toString("hh:mm:ss.zzz") + "/" + length.Time.toString("hh:mm:ss.zzz");
	message += "\tFrame: " + QString::number(position.Frame);
	message += "\tName: " + ui->video->metadata().getFileName();

	ui->statusBar->showMessage(message);
}

void MainWindow::positionUpdate()
{
	GstTime pos, len;

	if (ui->video->state() != QGst::StateReady &&
	    ui->video->state() != QGst::StateNull)
	{
		pos = ui->video->position();
		len = ui->video->length();
	}
	updateStatus(pos, len);
}

void MainWindow::stateUpdate()
{
	QGst::State newState = ui->video->state();
	// TODO: set controls state
	ui->controls->setVolume(QGst::StreamVolume::convert(QGst::StreamVolumeFormatCubic,
	                        QGst::StreamVolumeFormatLinear,
	                        ui->video->volume()) * 100);

	switch (newState)
	{
	case QGst::StateNull:
		ui->video->update();
	//case QGst::StatePaused:
		positionUpdate();
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
	playFile(url);
}

void MainWindow::toggleeditor()
{
	if (editor)
	{
		ui->editor->hide();
		ui->centralWidget->setFocus();
	}
	else
	{
		ui->editor->show();
		ui->editor->setFocus();
	}
	editor = !editor;
}

void MainWindow::fullScreen()
{
	setWindowState(windowState() ^ Qt::WindowFullScreen);

	static bool a = !settings->Gui.ControlBar;
	static bool b = !settings->Gui.StatusBar;
	static bool c = !settings->Gui.Editor;

	if (isFullScreen())
	{
		a = ui->controls->isHidden();
		b = ui->statusBar->isHidden();
		c = ui->editor->isHidden();
		ui->controls->hide();
		ui->statusBar->hide();
		ui->editor->hide();
	}
	else
	{
		if (!a) ui->controls->show();
		if (!b) ui->statusBar->show();
		if (!c) ui->editor->show();
	}
}

void MainWindow::seekForward()
{
	if (ui->video->state() != QGst::StateReady &&
	    ui->video->state() != QGst::StateNull)
	{
		seek(settings->Video.SeekShort);
	}
}

void MainWindow::seekBackward()
{
	if (ui->video->state() != QGst::StateReady &&
	    ui->video->state() != QGst::StateNull)
	{
		seek(-1 * settings->Video.SeekShort);
	}
}

void MainWindow::seekFrame(qint32 frame)
{
	if (ui->video->state() != QGst::StateReady &&
	    ui->video->state() != QGst::StateNull)
	{
		gotoFrame(frame, false);
	}
}

void MainWindow::nextFrame()
{
	if (ui->video->state() != QGst::StateReady &&
	    ui->video->state() != QGst::StateNull)
	{
		gotoFrame(ui->video->position().Frame + 1, true);
	}
}

void MainWindow::prevFrame()
{
	if (ui->video->state() != QGst::StateReady &&
	    ui->video->state() != QGst::StateNull)
	{
		gotoFrame(ui->video->position().Frame - 1, true);
	}
}

void MainWindow::volumeUp()
{
	double vol = ui->video->volume();
	if (vol < 1.0)
	{
		double linear = QGst::StreamVolume::convert(QGst::StreamVolumeFormatCubic, QGst::StreamVolumeFormatLinear, vol);
		double newVol = linear + MIN(1.0 - linear, 0.05);
		ui->video->setVolume(QGst::StreamVolume::convert(QGst::StreamVolumeFormatLinear, QGst::StreamVolumeFormatCubic, newVol));
		ui->controls->setVolume(newVol * 100);
	}
}

void MainWindow::volumeDown()
{
	double vol = ui->video->volume();
	if (vol > 0.0)
	{
		double linear = QGst::StreamVolume::convert(QGst::StreamVolumeFormatCubic, QGst::StreamVolumeFormatLinear, vol);
		double newVol = linear - MIN(linear, 0.05);
		ui->video->setVolume(QGst::StreamVolume::convert(QGst::StreamVolumeFormatLinear, QGst::StreamVolumeFormatCubic, newVol));
		ui->controls->setVolume(newVol * 100);
	}
}

void MainWindow::hueUp()
{
	ui->video->setHue(ui->video->hue() + 0.1);
}

void MainWindow::hueDown()
{
	ui->video->setHue(ui->video->hue() - 0.1);
}

void MainWindow::saturationUp()
{
	ui->video->setSaturation(ui->video->saturation() + 0.1);
}

void MainWindow::saturationDown()
{
	ui->video->setSaturation(ui->video->saturation() - 0.1);
}

void MainWindow::brightnessUp()
{
	ui->video->setBrightness(ui->video->brightness() + 0.1);
}

void MainWindow::brightnessDown()
{
	ui->video->setBrightness(ui->video->brightness() - 0.1);
}

void MainWindow::contrastUp()
{
	ui->video->setContrast(ui->video->contrast() + 0.1);
}

void MainWindow::contrastDown()
{
	ui->video->setContrast(ui->video->contrast() - 0.1);
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
