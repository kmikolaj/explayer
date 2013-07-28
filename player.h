#ifndef PLAYER_H
#define PLAYER_H

#include <QTimer>
#include <QTime>
#include <QGst/Pipeline>
#include <QGst/Ui/VideoWidget>
#include <QGst/Discoverer>
#include <QUrl>
#include "settings.h"
#include "osd.h"

enum SeekFlag
{
	None,
	Accurate,
	Skip
};

class MetaData
{
public:
	MetaData() : valid(false) {}
	MetaData(const QGst::DiscovererInfoPtr& _info);
	double getFramerate() const {return framerate; }
	QTime getDuration() const { return duration; }
	quint32 getFrames() const { return frames; }
	quint64 getSize() const { return size; }
	QString getFileName() const { return filename; }
	bool isValid() const { return valid; }
private:
	QGst::DiscovererInfoPtr info;
	QGst::DiscovererVideoInfoPtr videoInfo;
	bool valid;
	double framerate;
	QTime duration;
	quint64 frames;
	quint64 size;
	QString filename;
};

class Player : public QGst::Ui::VideoWidget
{
    Q_OBJECT

public:
    Player(QWidget *parent = 0);
    ~Player();

    void setUri(const QString & uri);
    void setSubtitles(const QString & sub, const QString & font, const QString & enc);

    QTime position() const;
	quint64 currentframe() const;
    void setPosition(const QTime & pos, SeekFlag flag=None);
    int volume() const;

    QGst::State state() const;
    QTime length() const;
	MetaData metadata() const;

	Osd *osd; // DEBUG

public slots:
    void play();
    void pause();
	void toggle();
    void stop();
	void gotoframe(quint64 frame);
    void setVolume(int volume);
	void forceaspectratio();
	void togglesubtitles();

signals:
    void positionChanged();
    void stateChanged();

private:
    void onBusMessage(const QGst::MessagePtr & message);
    void handlePipelineStateChange(const QGst::StateChangedMessagePtr & scm);
	quint64 toFrame(const QTime& time) const;
	void extractMetaData();

    QGst::PipelinePtr pipeline;
	QString videouri;
	QString suburi;
	bool subtitles;
	bool aspectratio;
	MetaData meta;
    QTimer positionTimer;
	Settings *settings;
};

#endif
