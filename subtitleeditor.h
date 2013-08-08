#ifndef SUBTITLEEDITOR_H
#define SUBTITLEEDITOR_H

#include <QPlainTextEdit>

class SubtitleEditor : public QPlainTextEdit
{
	Q_OBJECT
public:
	explicit SubtitleEditor(QWidget *parent = 0);

protected:
    virtual void keyPressEvent(QKeyEvent *e);

private:
	void highlightLine();

signals:
	void hideWindow();

public slots:
	void on_cursorPositionChanged();
};

#endif // SUBTITLEEDITOR_H
