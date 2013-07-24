#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>


class Settings : public QObject
{
public:
	static Settings* GetSettings(QObject *parent = 0);

	struct
	{
		QString Output;
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

private:
	Settings(QObject *parent = 0);
	~Settings();

	void setDefault();
};

#endif // SETTINGS_H
