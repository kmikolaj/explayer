#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>


class Settings : public QObject
{
public:
	static Settings *GetSettings(QObject *parent = 0);

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
		bool Osd;
		bool ControlBar;
		bool StatusBar;
		bool MenuBar;
		bool Editor;
	} Gui;

private:
	Settings(QObject *parent = 0);
	~Settings();

	void setDefault();
};

#endif // SETTINGS_H
