#include "linetextrenderstep.h"

#include "libcontemporary_global.h"
#include "texteditor.h"
#include "texteditor_p.h"
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

    painter->setFont(editor->font());
    QRect lineTextRect;
    QString lineText = editor->d->lines.at(line)->contents;
    lineTextRect.setWidth(editor->fontMetrics().horizontalAdvance(lineText));
    lineTextRect.setHeight(editor->fontMetrics().height());
    lineTextRect.moveCenter(lineRect.center());
    lineTextRect.moveLeft(outputBounds.left() - editor->horizontalScrollBar()->value());

    painter->setClipRect(outputBounds);
    painter->setClipping(true);
    painter->drawText(lineTextRect, Qt::AlignVCenter | Qt::AlignLeft, lineText);
    painter->restore();
}
