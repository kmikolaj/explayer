#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	connect(ui->video, SIGNAL(positionChanged()), this, SLOT(positionChanged()));
	connect(ui->video, SIGNAL(stateChanged()), this, SLOT(setState()));

	movieDir = ".";
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
	ui->video->play();
}

void MainWindow::playUrl(const QUrl &url)
{
	ui->video->stop();
	ui->video->setUri(url.toEncoded());
	ui->video->play();
}

void MainWindow::gotoFrame(qint64 frame, bool pause)
{
	if (ui->video->state() != QGst::StateNull)
	{
		double fps = ui->video->getMetaData().getFramerate();
		double res = (double(frame) / fps) * 1000.0;

		if (ui->video->length() != QTime(0,0))
		{
	        QTime pos(0,0);
			pos = pos.addMSecs(qint64(res));

			ui->video->setPosition(pos, Accurate | Skip);
			if (pause)
				ui->video->pause();
		}
	}
}

void MainWindow::gotoTime(qint64 time, bool pause)
{
	if (ui->video->state() != QGst::StateNull)
	{
		uint length = ui->video->length().msecsTo(QTime(0,0));
	    if (length != 0)
		{
	        QTime pos(0,0);
//			pos = pos.addMSecs(qint64(res));

			ui->video->setPosition(pos);
			if (pause)
				ui->video->pause();
		}
	}
}

void MainWindow::positionChanged()
{
	QTime pos(0,0);
	QTime len(0,0);
	double fps = 0.0;


	if (ui->video->state() != QGst::StateReady &&
        ui->video->state() != QGst::StateNull)
	{
		fps = ui->video->metadata().getFramerate();
		pos = ui->video->position();
		len = ui->video->length();
	}

	quint32 frame = double(-1 * pos.msecsTo(QTime(0,0))) * fps / 1000.0;
	QString message = pos.toString("hh:mm:ss.zzz") + "/" + len.toString("hh:mm:ss.zzz");
	message += " Frame: " + QString::number(frame);
	message += " Name: " + ui->video->metadata().getFileName();
	message += " Frames: " + QString::number(ui->video->metadata().getFrames());

	ui->statusBar->showMessage(message);
}

void MainWindow::setState()
{
	//
}

//void MainWindow::on_pushButton_clicked()
//{
//	openFile();
//}

//void MainWindow::on_pushButton_2_clicked()
//{
//	gotoFrame(2870, true);
//}
