#include "settings.h"

Settings* Settings::GetSettings(QObject *parent)
{
	static Settings* settings = NULL;
	if (!settings)
	{
		settings = new Settings(parent);
	}
	return settings;
}

Settings::Settings(QObject *parent) : QObject(parent)
{
	setDefault();
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

	// audio
	Audio.Output = "alsa";

	// subtitles
	Subtitles.Encoding = "cp1250";
	Subtitles.Font = "DejaVu Sans 14";

	// gui
	Gui.VideoDir = ".";
	Gui.Osd = true;
	Gui.ControlBar = true;
	Gui.StatusBar = true;
	Gui.MenuBar = false;
	Gui.Editor = false;
}
