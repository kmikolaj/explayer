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
}

Settings::~Settings()
{
}

void Settings::setDefault()
{
	// video
	Video.Output = "xv";

	// audio
	Audio.Output = "alsa";

	// subtitles
	Subtitles.Encoding = "cp1250";
	Subtitles.Font = "DejaVu Sans 14";
}
