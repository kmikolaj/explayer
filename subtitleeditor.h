#ifndef SUBTITLEEDITOR_H
#define SUBTITLEEDITOR_H

#include "settings.h"
#include <QPlainTextEdit>

class SubtitleEditor : public QPlainTextEdit
{
	Q_OBJECT
public:
	explicit SubtitleEditor(QWidget *parent = 0);
	void loadSubtitles(const QString &filename);
	void saveSubtitles(const QString &filename);

protected:
	virtual void keyPressEvent(QKeyEvent *e);

private:
	Settings *settings;
	void highlightLine();

signals:
	void hideWindow();

public slots:
	void on_cursorPositionChanged();
};

#endif // SUBTITLEEDITOR_H
