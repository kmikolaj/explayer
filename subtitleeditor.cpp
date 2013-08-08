#include "subtitleeditor.h"
#include <QDebug>

SubtitleEditor::SubtitleEditor(QWidget *parent) :
	QPlainTextEdit(parent)
{
	setViewportMargins(-4, -5, -4, -5);
	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(on_cursorPositionChanged()));
	highlightLine();
}

void SubtitleEditor::keyPressEvent(QKeyEvent *e)
{
	// przechwyć taba
	// emit
	if (e->key() == Qt::Key_Tab)
	{
		emit hideWindow();
	}
	else
		QPlainTextEdit::keyPressEvent(e);
}

void SubtitleEditor::highlightLine()
{
	QTextEdit::ExtraSelection highlight;
	highlight.cursor = textCursor();
	highlight.format.setProperty(QTextFormat::FullWidthSelection, true);
	highlight.format.setBackground(Qt::green);

	QList<QTextEdit::ExtraSelection> extras;
	extras << highlight;
	setExtraSelections(extras);
}

void SubtitleEditor::on_cursorPositionChanged()
{
	highlightLine();
}
