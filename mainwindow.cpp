#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>

#include <QPainter>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	settings = Settings::GetSettings(this);

	connect(ui->video, SIGNAL(positionChanged()), this, SLOT(positionUpdate()));
	connect(ui->video, SIGNAL(stateChanged()), this, SLOT(stateUpdate()));

	connect(ui->controls, SIGNAL(play()), ui->video, SLOT(play()));
	connect(ui->controls, SIGNAL(pause()), ui->video, SLOT(pause()));
	connect(ui->controls, SIGNAL(stop()), ui->video, SLOT(stop()));
	connect(ui->controls, SIGNAL(open()), this, SLOT(open()));
	connect(ui->controls, SIGNAL(volumeChanged(int)), ui->video, SLOT(setVolume(int)));

	movieDir = settings->Gui.VideoDir;

	// hotkeys
	addHotkey("Space", ui->video, SLOT(toggle()));
	addHotkey("f", this, SLOT(fullScreen()));
	addHotkey("Tab", this, SLOT(toggleeditor()));
	addHotkey("s", ui->video, SLOT(togglesubtitles()));
	addHotkey("r", ui->video, SLOT(forceaspectratio()));
	addHotkey("Right", this, SLOT(seekForward()));
	addHotkey("Left", this, SLOT(seekBackward()));
	addHotkey("h", this, SLOT(volumeUp()));
	addHotkey("g", this, SLOT(jump()));
	addHotkey(".", this, SLOT(nextFrame()));
	addHotkey(",", this, SLOT(prevFrame()));

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
	QString file = QFileDialog::getOpenFileName(this, tr("Open Video File"), movieDir);
	if (!file.isEmpty())
		playFile(file);
}

void MainWindow::playFile(const QString &filename)
{
	ui->video->stop();
	ui->video->setUri(filename);
	QString subfilename = filename.left(filename.lastIndexOf('.')) + ".txt";
	ui->video->setSubtitles(subfilename, settings->Subtitles.Font, settings->Subtitles.Encoding);
	ui->video->play();
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

void MainWindow::seek(int seconds)
{
	GstTime newPos = ui->video->position();
	newPos.moveMsec(1000 * seconds);
	if (newPos.Time >= QTime(0,0) && newPos.Time <= ui->video->length().Time)
	{
//		ui->video->setPosition(newPos, SeekFlag(Accurate));
		ui->video->setPosition(newPos);
		updateStatus(newPos, ui->video->length());
	}
}

void MainWindow::gotoFrame(qint32 frame, bool pause)
{
	if (ui->video->state() != QGst::StateNull)
	{
		GstTime newPos = GstTime(frame);
		if (ui->video->length().Time > QTime(0,0))
		{
			ui->video->setPosition(newPos, SeekFlag(Accurate | Skip));
			updateStatus(newPos, ui->video->length());
			if (pause)
				ui->video->pause();
		}
	}
}

void MainWindow::gotoTime(qint64 time, bool pause)
{
	if (ui->video->state() != QGst::StateNull)
	{
//		if (length != 0)
		{

//			ui->video->setPosition(pos);
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
	ui->controls->setVolume(ui->video->volume());
	if (newState == QGst::StateNull)
	{
		positionUpdate();
		qDebug() << "STATE NULL";
	}
}

void MainWindow::open()
{
	openFile();
}

void MainWindow::toggleeditor()
{
	if (editor)
		ui->editor->hide();
	else
		ui->editor->show();
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
	int vol = ui->video->volume();
	if (vol < 10)
	{
		ui->video->setVolume(vol + 1);
		ui->controls->setVolume(vol - 1);
	}
}

void MainWindow::volumeDown()
{
	int vol = ui->video->volume();
	if (vol > 0)
	{
		ui->video->setVolume(vol - 1);
		ui->controls->setVolume(vol - 1);
	}
}

void MainWindow::jump()
{
	gotoFrame(2870, true);
}
