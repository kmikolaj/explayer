#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	connect(ui->videoSurface, SIGNAL(positionChanged()), this, SLOT(positionChanged()));
	connect(ui->videoSurface, SIGNAL(stateChanged()), this, SLOT(setState()));

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
	ui->videoSurface->stop();
	ui->videoSurface->setUri(filename);
	ui->videoSurface->play();
}

void MainWindow::playUrl(const QUrl &url)
{
	ui->videoSurface->stop();
	ui->videoSurface->setUri(url.toEncoded());
	ui->videoSurface->play();
}

void MainWindow::gotoFrame(qint64 frame, bool pause)
{
	if (ui->videoSurface->state() != QGst::StateNull)
	{
		double fps = ui->videoSurface->getMetaData().getFramerate();
		double res = (double(frame) / fps) * 1000.0;

		uint length = ui->videoSurface->length().msecsTo(QTime(0,0));
	    if (length != 0)
		{
	        QTime pos(0,0);
			pos = pos.addMSecs(qint64(res));

			ui->videoSurface->setPosition(pos, Accurate | Skip);
			if (pause)
				ui->videoSurface->pause();
		}
	}
}

void MainWindow::gotoTime(qint64 time, bool pause)
{
	if (ui->videoSurface->state() != QGst::StateNull)
	{
		uint length = ui->videoSurface->length().msecsTo(QTime(0,0));
	    if (length != 0)
		{
	        QTime pos(0,0);
//			pos = pos.addMSecs(qint64(res));

			ui->videoSurface->setPosition(pos);
			if (pause)
				ui->videoSurface->pause();
		}
	}
}

void MainWindow::positionChanged()
{
//	double pp = double(pos) / 1000.0;
//	double fps = 23.976;
//	double res = pp * fps;
//	ui->statusBar->showMessage(QString::number(qint64(res)));
}

void MainWindow::setState()
{
	//
}

void MainWindow::on_pushButton_clicked()
{
	openFile();
}

void MainWindow::on_pushButton_2_clicked()
{
	gotoFrame(2870, true);
}
