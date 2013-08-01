#include "player.h"
#include <QDir>
#include <QUrl>
#include <QGlib/Connect>
#include <QGlib/Error>
#include <QGst/ElementFactory>
#include <QGst/Bus>
#include <QGst/Message>
#include <QGst/Query>
#include <QGst/ClockTime>
#include <QGst/Event>
#include <QGst/StreamVolume>
#include <QGst/Caps>

double GstTime::framerate = 0.0;

Player::Player(QWidget *parent)
	: QGst::Ui::VideoWidget(parent)
{
	//this timer is used to tell the ui to change its position slider & label
	//every 100 ms, but only when the pipeline is playing
	connect(&positionTimer, SIGNAL(timeout()), this, SIGNAL(positionChanged()));
	settings = Settings::GetSettings(this);
	subtitles = settings->Video.Subtitles;
	aspectratio = settings->Video.ForceAspectRatio;
}

Player::~Player()
{
	if (pipeline)
	{
		pipeline->setState(QGst::StateNull);
		stopPipelineWatch();
	}
}

void Player::setUri(const QString &uri)
{
	QString realUri = uri;

	//if uri is not a real uri, assume it is a file path
	if (realUri.indexOf("://") < 0)
	{
		realUri = QUrl::fromLocalFile(realUri).toString();
	}

	if (!pipeline)
	{
		pipeline = QGst::ElementFactory::make("playbin2").dynamicCast<QGst::Pipeline>();
		osd = new Osd(pipeline, this);
		if (settings->Gui.Osd)
			osd->enable();

		if (pipeline)
		{
			//let the video widget watch the pipeline for new video sinks
			watchPipeline(pipeline);

			//watch the bus for messages
			QGst::BusPtr bus = pipeline->bus();
			bus->addSignalWatch();
			QGlib::connect(bus, "message", this, &Player::onBusMessage);
		}
		else
		{
			qCritical() << "Failed to create the pipeline";
		}
	}

	if (pipeline)
	{
		videouri = realUri;
		pipeline->setProperty("uri", videouri);
		extractMetaData();
		GstTime::setFps(meta.getFramerate());
	}
}

void Player::setSubtitles(const QString &sub, const QString &font, const QString &enc)
{
	QString realUri = sub;

	//if uri is not a real uri, assume it is a file path
	if (realUri.indexOf("://") < 0)
	{
		realUri = QUrl::fromLocalFile(realUri).toString();
	}

	if (pipeline)
	{
		suburi = realUri;
		if (subtitles)
		{
			pipeline->setProperty("suburi", suburi);
		}
		pipeline->setProperty("subtitle-encoding", enc);
		pipeline->setProperty("subtitle-font-desc", font);
	}
}

void Player::extractMetaData()
{
	if (pipeline)
	{
		// create a discoverer
		QGst::DiscovererPtr discoverer = QGst::Discoverer::create(QGst::ClockTime::fromSeconds(1));
		QGst::DiscovererInfoPtr info;

		try
		{
			info = discoverer->discoverUri(videouri);
			meta = MetaData(info);
			return;
		}
		catch (const QGlib::Error &error)
		{
			qDebug() << "Failed to discover metadata";
		}
	}
	meta = MetaData();
}

GstTime Player::position() const
{
	if (pipeline)
	{
		QGst::PositionQueryPtr query = QGst::PositionQuery::create(QGst::FormatTime);
		pipeline->query(query);
		return GstTime(QGst::ClockTime(query->position()).toTime());
	}
	else
	{
		return GstTime();
	}
}

void Player::setPosition(const GstTime &pos, SeekFlag flag)
{
	if (pipeline)
	{
		QGst::SeekFlags flags = QGst::SeekFlagFlush;
		if (flag & Accurate)
			flags |= QGst::SeekFlagAccurate;
		if (flag & Skip)
			flags |= QGst::SeekFlagSkip;

		QGst::SeekEventPtr evt = QGst::SeekEvent::create(
		                             1.0, QGst::FormatTime, flags,
		                             QGst::SeekTypeSet, QGst::ClockTime::fromTime(pos.Time),
		                             QGst::SeekTypeNone, QGst::ClockTime::None
		                         );

		pipeline->sendEvent(evt);
	}
}

int Player::volume() const
{
	if (pipeline)
	{
		QGst::StreamVolumePtr svp =
		    pipeline.dynamicCast<QGst::StreamVolume>();

		if (svp)
		{
			return svp->volume(QGst::StreamVolumeFormatCubic) * 10;
		}
	}

	return 0;
}

void Player::setVolume(int volume)
{
	if (pipeline)
	{
		QGst::StreamVolumePtr svp =
		    pipeline.dynamicCast<QGst::StreamVolume>();

		if (svp)
		{
			svp->setVolume((double)volume / 10.0, QGst::StreamVolumeFormatCubic);
		}
	}
}

void Player::forceaspectratio()
{
	if (pipeline)
	{
		aspectratio = !aspectratio;
		QGst::ElementPtr sink = pipeline->property("video-sink").get<QGst::ElementPtr>();
		if (sink)
		{
			QGst::ChildProxyPtr proxy = sink.dynamicCast<QGst::ChildProxy>();
			proxy->childByIndex(0)->setProperty("force-aspect-ratio", aspectratio);
		}
		if (aspectratio)
		{
			osd->setText("keep aspect ratio");
		}
		else
		{
			osd->setText("ignore aspect ratio");
		}
		this->repaint();
	}
}

void Player::togglesubtitles()
{
	// does not work
	// https://bugzilla.gnome.org/show_bug.cgi?id=589515
	if (pipeline)
	{
		if (subtitles)
		{
			osd->setText("no subtitles");
			pipeline->setProperty("suburi", NULL);
		}
		else
		{
			osd->setText("subtitles");
			pipeline->setProperty("suburi", suburi);
		}
		subtitles = !subtitles;
	}
}

GstTime Player::length() const
{
	return (meta.isValid() ? meta.getDuration() : GstTime());
}

QGst::State Player::state() const
{
	return pipeline ? pipeline->currentState() : QGst::StateNull;
}

MetaData Player::metadata() const
{
	return meta;
}

void Player::play()
{
	if (pipeline)
	{
		pipeline->setState(QGst::StatePlaying);
	}
}

void Player::pause()
{
	if (pipeline)
	{
		pipeline->setState(QGst::StatePaused);
	}
}

void Player::toggle()
{
	switch (state())
	{
	case QGst::StatePlaying:
		pause();
		break;
	case QGst::StatePaused:
		play();
		break;
	default:
		break;
	}
}

void Player::stop()
{
	if (pipeline)
	{
		pipeline->setState(QGst::StateNull);
		emit stateChanged();
	}
}

void Player::gotoframe(quint64 frame)
{
	if (pipeline)
	{
		if (state() == QGst::StatePlaying)
			pause();
	}
}

void Player::onBusMessage(const QGst::MessagePtr &message)
{
	switch (message->type())
	{
	case QGst::MessageEos: //End of stream. We reached the end of the file.
		stop();
		break;
	case QGst::MessageError: //Some error occurred.
		qCritical() << message.staticCast<QGst::ErrorMessage>()->error();
		stop();
		break;
	case QGst::MessageStateChanged: //The element in message->source() has changed state
		if (message->source() == pipeline)
		{
			handlePipelineStateChange(message.staticCast<QGst::StateChangedMessage>());
		}
		break;
	default:
		break;
	}
}

void Player::handlePipelineStateChange(const QGst::StateChangedMessagePtr &scm)
{
	switch (scm->newState())
	{
	case QGst::StatePlaying:
		//start the timer when the pipeline starts playing
		positionTimer.start(100);
		break;
	case QGst::StatePaused:
		//stop the timer when the pipeline pauses
		if (scm->oldState() == QGst::StatePlaying)
		{
			positionTimer.stop();
		}
		break;
	case QGst::StateReady:
	{
		QGst::ElementPtr sink = pipeline->property("video-sink").get<QGst::ElementPtr>();
		if (sink)
		{
			QGst::ChildProxyPtr proxy = sink.dynamicCast<QGst::ChildProxy>();
			proxy->childByIndex(0)->setProperty("force-aspect-ratio", aspectratio);
		}
		break;
	}
	case QGst::StateNull:
		positionTimer.stop();
		break;
	default:
		break;
	}

	emit stateChanged();
}


MetaData::MetaData(const QGst::DiscovererInfoPtr &_info) : info(_info)
{
	if (info->videoStreams().size() > 0)
	{
		videoInfo = info->videoStreams()[0].dynamicCast<QGst::DiscovererVideoInfo>();

		framerate = double(videoInfo->framerate().numerator) /
		            double(videoInfo->framerate().denominator);
		duration = QGst::ClockTime(info->duration()).toTime();
		// TODO:miliseconds
//		frames = duration.hour() * 3600 + duration.minute() * 60 + duration.second();
//		frames = quint64(frames * framerate + 0.5);

		filename = info->uri().toLocalFile();
		QFile file(filename);
		size = file.size();
		valid = true;
	}
	else
	{
		init();
	}
}

MetaData::MetaData()
{
	init();
}

void MetaData::init()
{
	valid = false;
	framerate = 0.0;
	duration = GstTime();
	size = 0;
	filename = "";
}

GstTime::GstTime()
{
	Time = QTime(0,0);
	Nsec = Frame = Msec = 0;
}

GstTime::GstTime(const QTime &time)
{
	Time = time;
	Msec = time.hour() * 3600000 + time.minute() * 60000 + time.second() * 1000 + time.msec();
	Frame = qint32(Msec * GstTime::framerate / 1000.0 + 0.5);
	Nsec = Msec * 1000;
}

GstTime::GstTime(const qint32 frame)
{
	Frame = frame;
	Msec = qint64((frame / GstTime::framerate) * 1000.0);
	Time = QTime(0,0).addMSecs(Msec);
	Nsec = Msec * 1000;
}

GstTime::GstTime(const qint64 msec)
{
	Msec = msec;
	Frame = qint32(Msec * GstTime::framerate / 1000.0 + 0.5);
	Time = QTime(0,0).addMSecs(Msec);
	Nsec = Msec * 1000;
}

void GstTime::setFps(double fps)
{
	GstTime::framerate = fps;
}

void GstTime::moveMsec(qint64 msec)
{
	Msec += msec;
	Time = Time.addMSecs(msec);
	Frame = qint32(Msec * GstTime::framerate / 1000.0 + 0.5);
	Nsec += Msec * 1000;
}

void GstTime::moveFrame(qint32 frame)
{
	Frame += frame;
	Msec = qint64((frame / GstTime::framerate) * 1000.0);
	Time = QTime(0,0).addMSecs(Msec);
	Nsec = Msec * 1000;
}
