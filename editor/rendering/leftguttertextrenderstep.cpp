#include "leftguttertextrenderstep.h"

#include "libcontemporary_global.h"
#include "texteditor.h"
#include "texteditor_p.h"
#include <QPainter>
#include <QRect>
#include <QScrollBar>

LeftGutterTextRenderStep::LeftGutterTextRenderStep(TextEditor* parent) :
    TextEditorRenderStep{parent} {
}

void LeftGutterTextRenderStep::drawLine(int line, QRect outputBounds, QPainter* painter) {
    TextEditor* editor = parentEditor();

    QRect lineRect;
    lineRect.setHeight(editor->lineHeight(line));
    lineRect.setWidth(editor->width());
    lineRect.moveTop(editor->lineTop(line) - editor->verticalScrollBar()->value());
    lineRect.moveLeft(outputBounds.left());

    QRect margin = lineRect;
    margin.setWidth(outputBounds.width());

    QFont marginFont = parentEditor()->font();
    marginFont.setPointSizeF(parentEditor()->font().pointSizeF() * 0.8);

    painter->setFont(marginFont);

    QRect marginTextRect;

    QString marginText = QString::number(line + 1);
    marginTextRect.setWidth(QFontMetrics(marginFont).horizontalAdvance(marginText));
    marginTextRect.setHeight(QFontMetrics(marginFont).height());
    marginTextRect.moveCenter(margin.center());
    marginTextRect.moveRight(margin.right() - SC_DPI_W(6, editor));
    painter->drawText(marginTextRect, Qt::AlignCenter, marginText);
}

TextEditorRenderStep::RenderSide LeftGutterTextRenderStep::renderSide() const {
    return Left;
}

int LeftGutterTextRenderStep::renderWidth() const {
    QString testString;
    testString.fill('0', QString::number(parentEditor()->d->lines.length()).length());
    return SC_DPI_W(20, parentEditor()) + parentEditor()->fontMetrics().horizontalAdvance(testString);
}

QString LeftGutterTextRenderStep::stepName() const {
    return "LeftGutterText";
}

uint LeftGutterTextRenderStep::priority() const {
    return LeftGutterText;
}

void LeftGutterTextRenderStep::paint(QPainter* painter, QRect outputBounds, QRect redrawBounds) {
    TextEditor* editor = parentEditor();

    int firstLine = editor->lineAtY(redrawBounds.top());
    if (firstLine == -1) firstLine = editor->firstLineOnScreen();
    int lastLine = editor->lineAtY(redrawBounds.bottom());
    if (lastLine == -1) lastLine = editor->lastLineOnScreen();
    for (int i = firstLine; i <= lastLine; i++) {
        drawLine(i, outputBounds, painter);
    }
}
