#ifndef SUBTITLEEDITOR_H
#define SUBTITLEEDITOR_H

#include "settings.h"
#include "player.h"
#include <QPlainTextEdit>

class SubtitleEditor : public QPlainTextEdit
{
	Q_OBJECT
public:
	explicit SubtitleEditor(QWidget *parent = 0);
	void loadSubtitles(const QString &filename);
	void saveSubtitles(const QString &filename);
	void setPlayer(Player *player);

protected:
	virtual void keyPressEvent(QKeyEvent *e);
	virtual void showEvent(QShowEvent *);

private:
	Settings *settings;
	Player *player;
	QMap<qint32, int> startFrames;
	void highlightLine(int line = -1);
	bool extractFrame(qint32 &frame, const QString &str);
	bool currentFrame(qint32 &frame);
	void replace(const QString &pattern, const QString &str, int n);
	void gotoLine(qint32 frame);

	QString subFilename;

signals:
	void hideWindow();
	void jump(qint32);
	void save();

public slots:
	void on_cursorPositionChanged();
	void on_save();
};

#endif // SUBTITLEEDITOR_H
