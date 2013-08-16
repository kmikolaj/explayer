#include "subtitleeditor.h"
#include <QTextStream>
#include <QTextCodec>
#include <QTextBlock>
#include <QRegExp>
#include <QDebug>

SubtitleEditor::SubtitleEditor(QWidget *parent) :
    QPlainTextEdit(parent), player(nullptr)
{
	setViewportMargins(-4, -5, -4, -5);
	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(on_cursorPositionChanged()));
	connect(this, SIGNAL(save()), this, SLOT(on_save()));
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

void SubtitleEditor::setPlayer(Player *player)
{
	this->player = player;
}

void SubtitleEditor::loadSubtitles(const QString &filename)
{
	QFile file(filename);
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		subFilename = filename;
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
	QKeySequence sequence(e->key() | int(e->modifiers()));
	qint32 frame;
	if (sequence == settings->KeysPlayer.Editor)
	{
		emit hideWindow();
	}
	else if (sequence == settings->KeysEditor.JumpToSub)
	{
		if (extractFrame(frame))
			emit jump(frame);
	}
	else if (sequence == settings->KeysEditor.ReplaceStartFrame)
	{
		if (currentFrame(frame))
		{
			replace("\\{(\\d+)\\}", QString::number(frame), 1);
		}
	}
	else if (sequence == settings->KeysEditor.ReplaceEndFrame)
	{
		if (currentFrame(frame))
		{
			replace("\\{(\\d+)\\}", QString::number(frame), 2);
		}
	}
	else if (sequence == settings->KeysEditor.SaveSub)
	{
		emit save();
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

bool SubtitleEditor::extractFrame(qint32& frame)
{
	QRegExp mdvd("\\s*\\{(\\d+)\\}\\s*\\{(\\d+)\\}.*");
	bool ok = mdvd.exactMatch(textCursor().block().text());
	if (ok)
	{
		frame = mdvd.cap(1).toInt(&ok);
	}
	return ok;
}

bool SubtitleEditor::currentFrame(qint32& frame)
{
	bool ok = false;
	frame = 0;
	if (player)
	{
		frame = player->position().Frame;
		ok = true;
	}
	return ok;
}

void SubtitleEditor::replace(const QString &pattern, const QString &str, int n)
{
	QRegExp mdvd(pattern);
	QTextCursor cursor = textCursor();
	QString rep = cursor.block().text();
	int c = 0;
	int s = -1;
	while ((s = mdvd.indexIn(rep, s + 1)) != -1)
	{
		if (++c == n)
		{
			cursor.beginEditBlock();
			rep.replace(s + 1, mdvd.cap(1).length(), str);
			cursor.movePosition(QTextCursor::StartOfBlock);
			cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
			cursor.removeSelectedText();
			cursor.insertText(rep);
			setTextCursor(cursor);
			cursor.endEditBlock();
			break;
		}
	}
}

void SubtitleEditor::on_cursorPositionChanged()
{
	highlightLine();
}

void SubtitleEditor::on_save()
{
	if (!subFilename.isEmpty())
		saveSubtitles(subFilename);
}
