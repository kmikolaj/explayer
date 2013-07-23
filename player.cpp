/*
    Copyright (C) 2010 Marco Ballesio <gibrovacco@gmail.com>
    Copyright (C) 2011 Collabora Ltd.
      @author George Kiagiadakis <george.kiagiadakis@collabora.co.uk>

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "player.h"
#include <QDir>
#include <QUrl>
#include <QGlib/Connect>
#include <QGlib/Error>
#include <QGst/Pipeline>
#include <QGst/ElementFactory>
#include <QGst/Bus>
#include <QGst/Message>
#include <QGst/Query>
#include <QGst/ClockTime>
#include <QGst/Event>
#include <QGst/StreamVolume>
#include <QGst/Caps>

Player::Player(QWidget *parent)
	: QGst::Ui::VideoWidget(parent)
{
	//this timer is used to tell the ui to change its position slider & label
	//every 100 ms, but only when the pipeline is playing
	connect(&positionTimer, SIGNAL(timeout()), this, SIGNAL(positionChanged()));
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
		realUri = QUrl::fromLocalFile(realUri).toEncoded();
	}

	if (!pipeline)
	{
		pipeline = QGst::ElementFactory::make("playbin2").dynamicCast<QGst::Pipeline>();
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
		this->uri = realUri;
		pipeline->setProperty("uri", realUri);
		meta = getMetaData();
	}
}

MetaData Player::getMetaData()
{
	if (pipeline)
	{
		// create a discoverer
		QGst::DiscovererPtr discoverer = QGst::Discoverer::create(QGst::ClockTime::fromSeconds(1));
		QGst::DiscovererInfoPtr info;

		try
		{
			info = discoverer->discoverUri(uri);
			return MetaData(info);
		}
		catch (const QGlib::Error &error)
		{
			qDebug() << "Dupa";
		}

	}
	return MetaData();
}

QTime Player::position() const
{
	if (pipeline)
	{
		QGst::PositionQueryPtr query = QGst::PositionQuery::create(QGst::FormatTime);
		pipeline->query(query);
		return QGst::ClockTime(query->position()).toTime();
	}
	else
	{
		return QTime(0, 0);
	}
}

void Player::setPosition(const QTime &pos, SeekFlag flag)
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
		                             QGst::SeekTypeSet, QGst::ClockTime::fromTime(pos),
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
			svp->setVolume((double)volume / 10, QGst::StreamVolumeFormatCubic);
		}
	}
}

QTime Player::length() const
{
	return (meta.isValid() ? meta.getDuration() : QTime(0, 0));
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

void Player::stop()
{
	if (pipeline)
	{
		pipeline->setState(QGst::StateNull);

		//once the pipeline stops, the bus is flushed so we will
		//not receive any StateChangedMessage about this.
		//so, to inform the ui, we have to emit this signal manually.
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
		positionTimer.start(100);
		break;
	case QGst::StatePaused:
		//stop the timer when the pipeline pauses
		if (scm->oldState() == QGst::StatePlaying)
		{
			positionTimer.stop();
		}
		else if (scm->oldState() == QGst::StateReady)
		{
			QGst::ElementPtr sink = pipeline->property("video-sink").get<QGst::ElementPtr>();
			if (sink)
			{
				QGst::ChildProxyPtr proxy = sink.dynamicCast<QGst::ChildProxy>();
				proxy->childByIndex(0)->setProperty("force-aspect-ratio", true);
			}
		}
		break;
	default:
		break;
	}

	emit stateChanged();
}


MetaData::MetaData(const QGst::DiscovererInfoPtr &_info) : valid(false), info(_info)
{
	if (info->videoStreams().size() > 0)
	{
		videoInfo = info->videoStreams()[0].dynamicCast<QGst::DiscovererVideoInfo>();

		framerate = double(videoInfo->framerate().numerator) /
		            double(videoInfo->framerate().denominator);
		duration = QGst::ClockTime(info->duration()).toTime();
		frames = duration.hour() * 3600 + duration.minute() * 60 + duration.second();
		frames = quint64(frames * framerate + 0.5);

		filename = info->uri().toLocalFile();
		QFile file(filename);
		if (file.open(QIODevice::ReadOnly))
			size = file.size();

		valid = true;
	}
}
