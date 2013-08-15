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
	void setPlayer(Player* player);

protected:
	virtual void keyPressEvent(QKeyEvent *e);

private:
	Settings *settings;
	Player *player;
	void highlightLine();
	bool extractFrame(qint32 &frame);
	bool currentFrame(qint32 &frame);
	void replace(const QString &pattern, const QString &str, int n);

signals:
	void hideWindow();
	void jump(qint32);

public slots:
	void on_cursorPositionChanged();
};

#endif // SUBTITLEEDITOR_H
