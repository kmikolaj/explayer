#ifndef CONTROLS_H
#define CONTROLS_H

#include <QWidget>

namespace Ui
{
class Controls;
}

class Controls : public QWidget
{
	Q_OBJECT

public:
	explicit Controls(QWidget *parent = 0);
	~Controls();
	void setVolume(int volume);

signals:
	void play();
	void pause();
	void stop();
	void nextFrame();
	void prevFrame();
	void open();
	void volumeChanged(double);

private slots:
	void on_playButton_clicked();
	void on_pauseButton_clicked();
	void on_stopButton_clicked();
	void on_prevFrameButton_clicked();
	void on_nextFrameButton_clicked();
	void on_openButton_clicked();

	void on_volumeSlider_sliderMoved(int position);

private:
	Ui::Controls *ui;
};

#endif // CONTROLS_H
