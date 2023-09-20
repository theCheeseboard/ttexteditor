#include "linetextrenderstep.h"

#include "libcontemporary_global.h"
#include "texteditor.h"
#include "texteditor_p.h"
#include "texteditorsyntaxhighlighter.h"
#include <QPainter>
#include <QRect>
#include <QScrollBar>

LineTextRenderStep::LineTextRenderStep(TextEditor* parent) :
    TextEditorPerLineRenderStep{parent} {
}

uint LineTextRenderStep::priority() const {
    return LineText;
}

TextEditorRenderStep::RenderSide LineTextRenderStep::renderSide() const {
    return Center;
}

int LineTextRenderStep::renderWidth() const {
    return 0;
}

QString LineTextRenderStep::stepName() const {
    return "LineText";
}

void LineTextRenderStep::paintLine(int line, QPainter* painter, QRect outputBounds, QRect redrawBounds) {
    TextEditor* editor = parentEditor();

    painter->save();

    QRect lineRect;
    lineRect.setHeight(editor->lineHeight(line));
    lineRect.setWidth(editor->width());
    lineRect.moveTop(editor->lineTop(line) - editor->verticalScrollBar()->value());
    lineRect.moveLeft(0 - editor->horizontalScrollBar()->value());

    auto theme = editor->d->highlighter.theme();
    auto x = theme.filePath();
    auto y = theme.name();
    auto z = theme.isValid();
    auto formats = editor->lineProperties(line, TextEditor::HighlightFormat);
    for (auto formatVariant : formats) {
        painter->save();
        //        auto formatStr = formatVariant.toString();

        //        auto parts = formatStr.split(",");
        //        auto offset = parts.at(0).toInt();
        //        auto length = parts.at(1).toInt();
        //        auto textFormatId = parts.at(2).toInt();

        //        auto textFormat = editor->d->highlighter.formatForId(textFormatId);
        auto format = formatVariant.value<TextEditorSyntaxHighlighterFormat>();
        auto textFormat = format.format;

        QRect lineTextRect;
        auto preText = editor->d->lines.at(line)->contents.left(format.offset);
        auto lineText = editor->d->lines.at(line)->contents.mid(format.offset, format.length);
        lineTextRect.setWidth(editor->fontMetrics().horizontalAdvance(lineText));
        lineTextRect.setHeight(editor->fontMetrics().height());
        lineTextRect.moveCenter(lineRect.center());
        lineTextRect.moveLeft(outputBounds.left() - editor->horizontalScrollBar()->value() + editor->fontMetrics().horizontalAdvance(preText));
        painter->setClipRect(outputBounds);
        painter->setClipping(true);

        auto fnt = editor->font();
        //        if (textFormat.hasTextColorOverride()) {
        painter->setPen(textFormat.textColor(theme));
        //        }
        if (textFormat.hasBoldOverride()) {
            fnt.setBold(textFormat.isBold(theme));
        }
        if (textFormat.hasItalicOverride()) {
            fnt.setItalic(textFormat.isItalic(theme));
        }
        if (textFormat.hasUnderlineOverride()) {
            fnt.setUnderline(textFormat.isUnderline(theme));
        }
        if (textFormat.hasStrikeThroughOverride()) {
            fnt.setStrikeOut(textFormat.isStrikeThrough(theme));
        }

        painter->setFont(fnt);
        painter->drawText(lineTextRect, Qt::AlignVCenter | Qt::AlignLeft, lineText);
        painter->restore();
    }

    painter->restore();
}
