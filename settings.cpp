#include "settings.h"
#include <QDir>
#include <QTextStream>

Settings *Settings::GetSettings(QObject *parent)
{
	static Settings *settings = NULL;
	if (!settings)
	{
		settings = new Settings(parent);
	}
	return settings;
}

QString Settings::GetVersionInfo()
{
	QString info;
	return info.sprintf("Version: %d.%d.%d (Codename: %s)", _MAJOR_, _MINOR_, _REVISION_, _CODENAME_);
}

Settings::Settings(QObject *parent) : QObject(parent)
{
	setDefault();
	readLastPositions();
}

Settings::~Settings()
{
}

void Settings::setDefault()
{
	// video
	Video.Output = "xv";
	Video.VAAPI = false;
	Video.ForceAspectRatio = true;
	Video.PixelAspectRatio = 1.0;
	Video.Subtitles = true;
	Video.SeekShort = 5;
	Video.SeekMid = 60;
	Video.SeekLong = 600;

	// audio
	Audio.Output = "alsa";

	// subtitles
	Subtitles.Encoding = "cp1250";
	Subtitles.Font = "DejaVu Sans 14";

	// gui
	Gui.VideoDir = ".";
	Gui.LastDir = ".";
	Gui.RememberDir = true;
	Gui.Osd = true;
	Gui.Color = true;
	Gui.ControlBar = true;
	Gui.StatusBar = true;
	Gui.MenuBar = false;
	Gui.Editor = false;
	Gui.RememberPosition = true;

	QString dir;
	if (getenv("XDG_CACHE_HOME"))
		dir = QString(getenv("XDG_CACHE_HOME")) + QString("/ExPlayer");
	else
		dir = QString(getenv("HOME")) + QString("/.cache/Explayer");
	QDir("/").mkpath(dir);
	Gui.LastPositionFile = dir + "/positions";

	// hotkeys
	Keys.VolumeUp = QKeySequence(Qt::KeypadModifier | Qt::Key_Asterisk);
	Keys.VolumeDown = QKeySequence(Qt::KeypadModifier | Qt::Key_Slash);
	Keys.Mute = QKeySequence(Qt::Key_M);
	Keys.FullScreen = QKeySequence(Qt::Key_F);
	Keys.Subtitles = QKeySequence(Qt::Key_S);
	Keys.AspectRatio = QKeySequence(Qt::Key_R);
	Keys.PlayPause = QKeySequence(Qt::Key_Space);
	Keys.Editor = QKeySequence(Qt::Key_Tab);
	Keys.Stop = QKeySequence(Qt::ShiftModifier | Qt::Key_S);
	Keys.SeekForward = QKeySequence(Qt::Key_Right);
	Keys.SeekBackward = QKeySequence(Qt::Key_Left);
	Keys.NextFrame = QKeySequence(Qt::Key_Period);
	Keys.PrevFrame = QKeySequence(Qt::Key_Comma);
	Keys.Time =  QKeySequence(Qt::Key_O);
	Keys.ContrastUp = QKeySequence(Qt::Key_1);
	Keys.ContrastDown = QKeySequence(Qt::Key_2);
	Keys.BrightnessDown = QKeySequence(Qt::Key_3);
	Keys.BrightnessUp = QKeySequence(Qt::Key_4);
	Keys.SaturationUp = QKeySequence(Qt::Key_5);
	Keys.SaturationDown = QKeySequence(Qt::Key_6);
	Keys.HueUp = QKeySequence(Qt::Key_7);
	Keys.HueDown = QKeySequence(Qt::Key_8);
	Keys.FrameJump = QKeySequence(Qt::ControlModifier | Qt::Key_G);
	Keys.TimeJump = QKeySequence(Qt::ControlModifier | Qt::Key_T);
}

void Settings::readLastPositions()
{
	QFile file(Gui.LastPositionFile);
	QList<QByteArray> line;
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		// while
		line = file.readLine().split('=');
		if (line.size() > 1)
		{
			QString filename = QString::fromLocal8Bit(line[0]);
			QString time = QString::fromLocal8Bit(line[1]);
			Gui.LastPosition[filename] = QTime::fromString(time);
		}
	}
}

void Settings::saveLastPositions()
{
	QFile file(Gui.LastPositionFile);
	if (file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QTextStream stream(&file);
		foreach(const QString & name, Gui.LastPosition.keys())
		{
			stream << name << "=" << Gui.LastPosition[name].toString();
		}
	}
}
