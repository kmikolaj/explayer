#include "player.h"
#include <QDir>
#include <QUrl>
#include <QGlib/Connect>
#include <QGlib/Error>
#include <QGst/ElementFactory>
#include <QGst/Bus>
#include <QGst/Message>
#include <QGst/Query>
#include <QGst/Event>
#include <QGst/StreamVolume>
#include <QGst/ColorBalance>
#include <QGst/Caps>

Player::Player(QWidget *parent)
	: QGst::Ui::VideoWidget(parent)
{
	connect(&positionTimer, SIGNAL(timeout()), this, SIGNAL(positionChanged()));
	settings = Settings::GetSettings(this);
	subtitles = settings->Video.Subtitles;
	aspectratio = settings->Video.ForceAspectRatio;
	setHardwareAcceleration(settings->Video.VAAPI);
	dpms.Disable();
}

Player::~Player()
{
	if (pipeline)
	{
		if (state() != QGst::StateNull)
			settings->updatePosition(videouri, position().Time);
		else
			settings->updatePosition(videouri);
		pipeline->setState(QGst::StateNull);
		stopPipelineWatch();
	}
	dpms.Restore();
}

void Player::setUri(const QString &uri)
{
//	QString realUri = (uri.indexOf("://") < 0 ? QUrl::fromLocalFile(uri).toString() : uri);
	QString realUri = uri;

	if (!pipeline)
	{
		pipeline = QGst::ElementFactory::make("playbin2").dynamicCast<QGst::Pipeline>();
		osd = new Osd(pipeline, this);
		if (settings->Gui.Color)
			balance = new VideoBalance(pipeline, this);
		balance->link(osd);
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
		qDebug() << "POS TRUE";
		this->setAttribute(Qt::WA_PaintOnScreen, true);
	}
}

void Player::setSubtitles(const QString &sub, const QString &font, const QString &enc)
{
//	QString realUri = (sub.indexOf("://") < 0 ? QUrl::fromLocalFile(sub).toString() : sub);

	if (pipeline)
	{
//		suburi = realUri;
		suburi = sub;
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

void Player::setHardwareAcceleration(bool enable)
{
	QGst::PluginFeaturePtr decoder = QGst::ElementFactory::find("vaapidecode");
	if (decoder)
	{
		decoder->setRank(enable ? QGst::RankPrimary : QGst::RankNone);
	}
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

		QGst::SeekEventPtr evt = QGst::SeekEvent::create(1.0, QGst::FormatTime,
		                         flags, QGst::SeekTypeSet,
		                         QGst::ClockTime::fromTime(pos.Time),
		                         QGst::SeekTypeNone, QGst::ClockTime::None);

		pipeline->sendEvent(evt);

	}
}

double Player::volume() const
{
	if (pipeline)
	{
		QGst::StreamVolumePtr svp =
		    pipeline.dynamicCast<QGst::StreamVolume>();

		if (svp)
		{
			return svp->volume(QGst::StreamVolumeFormatCubic);
		}
	}
	return 0.0;
}

double Player::hue() const
{
	if (pipeline && balance && settings->Gui.Color)
	{
		return balance->hue();
	}
	return 0.0;
}

double Player::saturation() const
{
	if (pipeline && balance && settings->Gui.Color)
	{
		return balance->saturation();
	}
	return 0.0;
}

double Player::brightness() const
{
	if (pipeline && balance && settings->Gui.Color)
	{
		return balance->brightness();
	}
	return 0.0;
}

double Player::contrast() const
{
	if (pipeline && balance && settings->Gui.Color)
	{
		return balance->contrast();
	}
	return 0.0;
}

void Player::setVolume(double volume)
{
	if (pipeline)
	{
		QGst::StreamVolumePtr svp =
		    pipeline.dynamicCast<QGst::StreamVolume>();

		if (svp)
		{
			double newVol = QGst::StreamVolume::convert(QGst::StreamVolumeFormatLinear,
			                                            QGst::StreamVolumeFormatCubic, volume);
			svp->setVolume(newVol, QGst::StreamVolumeFormatCubic);
			osd->setText("Volume: " + QString::number(int(volume * 100)));
		}
	}
}

void Player::setHue(double hue)
{
	if (pipeline && balance && settings->Gui.Color)
	{
		balance->setHue(hue);
		osd->setText("Hue: " + QString::number(int(hue * 100)));
	}
}

void Player::setSaturation(double saturation)
{
	if (pipeline && balance && settings->Gui.Color)
	{
		balance->setSaturation(saturation);
		osd->setText("Saturation: " + QString::number(int(saturation * 100)));
	}
}

void Player::setBrightness(double brightness)
{
	if (pipeline && balance && settings->Gui.Color)
	{
		balance->setBrightness(brightness);
		osd->setText("Brightness: " + QString::number(int(brightness * 100)));
	}
}

void Player::setContrast(double contrast)
{
	if (pipeline && balance && settings->Gui.Color)
	{
		balance->setContrast(contrast);
		osd->setText("Contrast: " + QString::number(int(contrast * 100)));
	}
}

void Player::forceaspectratio()
{
	if (pipeline)
	{
		QGst::ElementPtr sink = videoSink();
		if (sink)
		{
			aspectratio = !aspectratio;
			sink->setProperty("force-aspect-ratio", aspectratio);
			if (aspectratio)
			{
				osd->setText("keep aspect ratio");
			}
			else
			{
				osd->setText("ignore aspect ratio");
			}
			update();
		}
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

void Player::toggletime()
{
	if (pipeline && osd)
	{
		osd->toggleTime();
	}
}

void Player::mute()
{
	if (pipeline)
	{
		QGst::StreamVolumePtr svp =
		    pipeline.dynamicCast<QGst::StreamVolume>();

		if (svp)
		{
			bool mute = svp->isMuted();
			svp->setMuted(!mute);
			osd->setText(mute ? "unmute" : "mute");
		}
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
		osd->setText("▶");
	}
}

void Player::play(QTime position)
{
	if (pipeline)
	{
		pipeline->setState(QGst::StatePlaying);
		startingPosition.Time = position;
		startingPosition.Changed = false;
		osd->setText("▶");
	}
}

void Player::pause()
{
	if (pipeline)
	{
		//osd->clear();
		//osd->setText("■"); nie działa
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
		qDebug() << "POS FALSE";
		this->setAttribute(Qt::WA_PaintOnScreen, false);
		emit stateChanged();
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
		positionTimer.start(int(1000.0 / meta.getFramerate()));
		break;
	case QGst::StatePaused:
		//stop the timer when the pipeline pauses
		if (scm->oldState() == QGst::StatePlaying)
		{
			positionTimer.stop();
		}
		else if (scm->oldState() == QGst::StateReady && !startingPosition.Changed)
		{
			setPosition(startingPosition.Time);
			startingPosition.Changed = true;
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
	}
	case QGst::StateNull:
		positionTimer.stop();
		break;
	default:
		break;
	}

	emit stateChanged();
}
