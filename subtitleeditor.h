#ifndef SUBTITLEEDITOR_H
#define SUBTITLEEDITOR_H

#include <QPlainTextEdit>

class SubtitleEditor : public QPlainTextEdit
{
	Q_OBJECT
public:
	explicit SubtitleEditor(QWidget *parent = 0);

private:
	void highlightLine();

signals:

public slots:
	void on_cursorPositionChanged();
};

#endif // SUBTITLEEDITOR_H
