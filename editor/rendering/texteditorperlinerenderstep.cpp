#include "texteditorperlinerenderstep.h"

#include "texteditor.h"

TextEditorPerLineRenderStep::TextEditorPerLineRenderStep(TextEditor* parent) :
    TextEditorRenderStep{parent} {
}

void TextEditorPerLineRenderStep::paint(QPainter* painter, QRect outputBounds, QRect redrawBounds) {
    TextEditor* parent = this->parentEditor();

    int firstLine = parent->lineAtY(redrawBounds.top());
    if (firstLine == -1) firstLine = parent->firstLineOnScreen();
    int lastLine = parent->lineAtY(redrawBounds.bottom());
    if (lastLine == -1) lastLine = parent->lastLineOnScreen();
    for (int i = firstLine; i <= lastLine; i++) {
        this->paintLine(i, painter, outputBounds, redrawBounds);
    }
}
