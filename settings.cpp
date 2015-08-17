#include "settings.h"
#include <QDir>
#include <QTextStream>

Settings &Settings::GetInstance()
{
	static Settings settings;
	return settings;
}

void Settings::updatePosition(const QString &file, const QTime &time)
{
	if (Gui.LastPosition.contains(file) && time == QTime(0, 0))
		Gui.LastPosition.remove(file);
	Gui.LastPosition[file] = time;
}

QTime Settings::getPosition(const QString &file)
{
	if (Gui.LastPosition.contains(file))
		return Gui.LastPosition[file];
	return QTime(0, 0);
}

Settings::Settings(QObject *parent) : QObject(parent)
{
	setDefault();
	if (Gui.RememberPosition)
		readLastPositions();
}

Settings::~Settings()
{
	if (Gui.RememberPosition)
		saveLastPositions();
}

void Settings::setDefault()
{
	// video
	Video.Output = "gstreamer";
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

	// hotkeys player
	KeysPlayer.VolumeUp = QKeySequence(Qt::KeypadModifier | Qt::Key_Asterisk);
	KeysPlayer.VolumeDown = QKeySequence(Qt::KeypadModifier | Qt::Key_Slash);
	KeysPlayer.Mute = QKeySequence(Qt::Key_M);
	KeysPlayer.FullScreen = QKeySequence(Qt::Key_F);
	KeysPlayer.Subtitles = QKeySequence(Qt::Key_S);
	KeysPlayer.AspectRatio = QKeySequence(Qt::Key_R);
	KeysPlayer.PlayPause = QKeySequence(Qt::Key_Space);
	KeysPlayer.Editor = QKeySequence(Qt::Key_Tab);
	KeysPlayer.Stop = QKeySequence(Qt::ShiftModifier | Qt::Key_S);
	KeysPlayer.SeekForward = QKeySequence(Qt::Key_Right);
	KeysPlayer.SeekBackward = QKeySequence(Qt::Key_Left);
	KeysPlayer.NextFrame = QKeySequence(Qt::Key_Period);
	KeysPlayer.PrevFrame = QKeySequence(Qt::Key_Comma);
	KeysPlayer.Time =  QKeySequence(Qt::Key_O);
	KeysPlayer.ContrastDown = QKeySequence(Qt::Key_1);
	KeysPlayer.ContrastUp = QKeySequence(Qt::Key_2);
	KeysPlayer.BrightnessDown = QKeySequence(Qt::Key_3);
	KeysPlayer.BrightnessUp = QKeySequence(Qt::Key_4);
	KeysPlayer.SaturationDown = QKeySequence(Qt::Key_5);
	KeysPlayer.SaturationUp = QKeySequence(Qt::Key_6);
	KeysPlayer.HueDown = QKeySequence(Qt::Key_7);
	KeysPlayer.HueUp = QKeySequence(Qt::Key_8);
	KeysPlayer.FrameJump = QKeySequence(Qt::ControlModifier | Qt::Key_G);
	KeysPlayer.TimeJump = QKeySequence(Qt::ControlModifier | Qt::Key_T);

	// hotkeys editor
	KeysEditor.JumpToSub = QKeySequence(Qt::ControlModifier | Qt::Key_P);
	KeysEditor.SaveSub = QKeySequence(Qt::ControlModifier | Qt::Key_S);
	KeysEditor.ReplaceStartFrame = QKeySequence(Qt::Key_Insert);
	KeysEditor.ReplaceEndFrame = QKeySequence(Qt::MetaModifier | Qt::AltModifier | Qt::Key_Insert);
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
		foreach (const QString &name, Gui.LastPosition.keys())
		{
			stream << name << "=" << Gui.LastPosition[name].toString();
		}
	}
}
