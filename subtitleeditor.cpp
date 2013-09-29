#include "subtitleeditor.h"
#include "utils.h"
#include <QTextStream>
#include <QTextCodec>
#include <QTextBlock>
#include <QRegExp>
#include <QDebug>

SubtitleEditor::SubtitleEditor(QWidget *parent) :
	QPlainTextEdit(parent)//, player(nullptr)
{
	setViewportMargins(-4, -5, -4, -5);
	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(on_cursorPositionChanged()));
	connect(this, SIGNAL(save()), this, SLOT(on_save()));
	setFont(QFont("monospace"));
	settings = Settings::GetSettings(this);
	if (!settings->Gui.Editor)
		hide();
}

void SubtitleEditor::saveSubtitles(const QString &filename)
{
	QFile file(filename);
	if (file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QTextCodec *codec = QTextCodec::codecForName(settings->Subtitles.Encoding.toLocal8Bit());
		QTextStream stream(&file);
		stream.setCodec(codec);
		stream << toPlainText();
	}
}
/*
void SubtitleEditor::setPlayer(Player *player)
{
	this->player = player;
}
*/
void SubtitleEditor::loadSubtitles(const QString &filename)
{
	QFile file(filename);
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		subFilename = filename;
		startFrames.clear();
		int i = 0;
		QTextCodec *codec = QTextCodec::codecForName(settings->Subtitles.Encoding.toLocal8Bit());
		QTextStream stream(&file);
		stream.setCodec(codec);
		while (!stream.atEnd())
		{
			qint32 frame;
			const QString &line = stream.readLine();
			appendPlainText(line);
			if (extractFrame(frame, line))
				startFrames[frame] = i++;
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
		if (extractFrame(frame, textCursor().block().text()))
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

void SubtitleEditor::showEvent(QShowEvent *e)
{
	qint32 frame;
	if (currentFrame(frame))
	{
		gotoLine(frame);
	}
	QPlainTextEdit::showEvent(e);
}

void SubtitleEditor::highlightLine(int line)
{
	QTextEdit::ExtraSelection highlight;
	if (line >= 0)
	{
		QTextCursor cursor = textCursor();
		cursor.movePosition(QTextCursor::Start);
		cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, line);
		setTextCursor(cursor);
	}
	highlight.cursor = textCursor();
	highlight.format.setProperty(QTextFormat::FullWidthSelection, true);
	highlight.format.setBackground(Qt::gray);

	QList<QTextEdit::ExtraSelection> extras;
	extras << highlight;
	setExtraSelections(extras);
}

bool SubtitleEditor::extractFrame(qint32 &frame, const QString &str)
{
	QRegExp mdvd("\\s*\\{(\\d+)\\}\\s*\\{(\\d+)\\}.*");
	bool ok = mdvd.exactMatch(str);
	if (ok)
	{
		frame = mdvd.cap(1).toInt(&ok);
	}
	return ok;
}

bool SubtitleEditor::currentFrame(qint32 &frame)
{
	bool ok = false;
	frame = 0;
//	if (player)
//	{
//		frame = player->position().Frame;
//		ok = true;
//	}
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

void SubtitleEditor::gotoLine(qint32 frame)
{
	if (startFrames.size() > 0)
	{
		auto it = startFrames.lowerBound(frame);
		int value;
		if (it != startFrames.end())
		{
			value = it.value();
		}
		else
		{
			it = startFrames.end() - 1;
			value = it.value() + 1;
		}
		int line = (it.key() == frame ? value : MAX(0, value - 1));
		highlightLine(line);
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
