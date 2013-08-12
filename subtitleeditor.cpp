#include "subtitleeditor.h"
#include <QTextStream>
#include <QTextCodec>
#include <QDebug>

SubtitleEditor::SubtitleEditor(QWidget *parent) :
	QPlainTextEdit(parent)
{
	setViewportMargins(-4, -5, -4, -5);
	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(on_cursorPositionChanged()));
	highlightLine();
	setFont(QFont("monospace"));
	settings = Settings::GetSettings(this);
}

void SubtitleEditor::saveSubtitles(const QString &filename)
{
	QFile file(filename);
	if (file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QTextCodec* codec = QTextCodec::codecForName(settings->Subtitles.Encoding.toLocal8Bit());
		QTextStream stream(&file);
		stream.setCodec(codec);
		stream << toPlainText();
	}
}

void SubtitleEditor::loadSubtitles(const QString &filename)
{
	QFile file(filename);
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QTextCodec* codec = QTextCodec::codecForName(settings->Subtitles.Encoding.toLocal8Bit());
		QTextStream stream(&file);
		stream.setCodec(codec);
		while(!stream.atEnd())
		{
			appendPlainText(stream.readLine());
		}
	}
}

void SubtitleEditor::keyPressEvent(QKeyEvent *e)
{
	if (QKeySequence(e->key()) == settings->Keys.Editor)
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
