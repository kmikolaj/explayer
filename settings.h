#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QKeySequence>
#include "version.h"

class Settings : public QObject
{
public:
	static Settings *GetSettings(QObject *parent = 0);
	static QString GetVersionInfo();

	struct
	{
		QString Output;
		bool VAAPI;
		bool ForceAspectRatio;
		double PixelAspectRatio;
		bool Subtitles;
		int SeekShort;
		int SeekMid;
		int SeekLong;
		bool RememberPosition;
	} Video;

	struct
	{
		QString Output;
	} Audio;

	struct
	{
		QString Font;
		QString Encoding;
	} Subtitles;

	struct
	{
		QString VideoDir;
		QString LastDir;
		bool RememberDir;
		bool Osd;
		bool Color;
		bool ControlBar;
		bool StatusBar;
		bool MenuBar;
		bool Editor;
	} Gui;

	struct
	{
		QKeySequence VolumeUp;
		QKeySequence VolumeDown;
		QKeySequence Mute;
		QKeySequence FullScreen;
		QKeySequence Subtitles;
		QKeySequence AspectRatio;
		QKeySequence PlayPause;
		QKeySequence Editor;
		QKeySequence Stop;
		QKeySequence SeekForward;
		QKeySequence SeekBackward;
		QKeySequence NextFrame;
		QKeySequence PrevFrame;
		QKeySequence BrightnessUp;
		QKeySequence BrightnessDown;
		QKeySequence ContrastUp;
		QKeySequence ContrastDown;
		QKeySequence SaturationUp;
		QKeySequence SaturationDown;
		QKeySequence HueUp;
		QKeySequence HueDown;
		QKeySequence Jump;
	} Keys;

private:
	Settings(QObject *parent = 0);
	~Settings();

	void setDefault();
};

#endif // SETTINGS_H
