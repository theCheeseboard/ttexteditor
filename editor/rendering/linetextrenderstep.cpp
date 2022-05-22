#include "linetextrenderstep.h"

#include "libcontemporary_global.h"
#include "texteditor.h"
#include "texteditor_p.h"
#include <QPainter>
#include <QRect>
#include <QScrollBar>

LineTextRenderStep::LineTextRenderStep(TextEditor* parent) :
    TextEditorRenderStep{parent} {
}

void LineTextRenderStep::drawLine(int line, QPainter* painter) {
    TextEditor* editor = parentEditor();

    QRect lineRect;
    lineRect.setHeight(editor->lineHeight(line));
    lineRect.setWidth(editor->width());
    lineRect.moveTop(editor->lineTop(line) - editor->verticalScrollBar()->value());
    lineRect.moveLeft(0 - editor->horizontalScrollBar()->value());

    QRect margin = lineRect;
    margin.setWidth(editor->leftMarginWidth());

    QFont marginFont = parentEditor()->font();
    marginFont.setPointSizeF(parentEditor()->font().pointSizeF() * 0.8);

    painter->setFont(marginFont);

    QRect marginTextRect;

    QString marginText = QString::number(line + 1);
    marginTextRect.setWidth(QFontMetrics(marginFont).horizontalAdvance(marginText));
    marginTextRect.setHeight(QFontMetrics(marginFont).height());
    marginTextRect.moveCenter(margin.center());
    marginTextRect.moveRight(margin.right() - SC_DPI_W(3, editor));
    painter->drawText(marginTextRect, Qt::AlignCenter, marginText);

    painter->setFont(editor->font());
    QRect lineTextRect;
    QString lineText = editor->d->lines.at(line)->contents;
    lineTextRect.setWidth(editor->fontMetrics().horizontalAdvance(lineText));
    lineTextRect.setHeight(editor->fontMetrics().height());
    lineTextRect.moveCenter(lineRect.center());
    lineTextRect.moveLeft(margin.right() + SC_DPI_W(3, editor));
    painter->drawText(lineTextRect, Qt::AlignVCenter | Qt::AlignLeft, lineText);
}

uint LineTextRenderStep::priority() const {
    return LineText;
}

void LineTextRenderStep::paint(QPainter* painter, QRect outputBounds, QRect redrawBounds) {
    TextEditor* editor = parentEditor();

    int firstLine = editor->lineAtY(redrawBounds.top());
    if (firstLine == -1) firstLine = editor->firstLineOnScreen();
    int lastLine = editor->lineAtY(redrawBounds.bottom());
    if (lastLine == -1) lastLine = editor->lastLineOnScreen();
    for (int i = firstLine; i <= lastLine; i++) {
        drawLine(i, painter);
    }
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
