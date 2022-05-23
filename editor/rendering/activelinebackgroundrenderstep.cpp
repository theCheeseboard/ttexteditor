#include "activelinebackgroundrenderstep.h"

#include "textcaret.h"
#include "texteditor.h"
#include "texteditor_p.h"
#include "texteditorcolorscheme.h"
#include <QPainter>
#include <QRect>

ActiveLineBackgroundRenderStep::ActiveLineBackgroundRenderStep(TextEditor* parent) :
    TextEditorPerLineRenderStep{parent} {
}

TextEditorRenderStep::RenderSide ActiveLineBackgroundRenderStep::renderSide() const {
    return Center;
}

int ActiveLineBackgroundRenderStep::renderWidth() const {
    return 0;
}

QString ActiveLineBackgroundRenderStep::stepName() const {
    return "ActiveLineBackground";
}

uint ActiveLineBackgroundRenderStep::priority() const {
    return LineActiveHighlight;
}

void ActiveLineBackgroundRenderStep::paintLine(int line, QPainter* painter, QRect outputBounds, QRect redrawBounds) {
    TextEditor* parent = parentEditor();

    painter->save();
    for (TextCaret* caret : parent->d->carets) {
        if (caret->firstAnchor() == caret->lastAnchor() && caret->linePos().y() == line) {
            QRect lineRect;
            lineRect.setHeight(parent->lineHeight(line));
            lineRect.moveTop(parent->lineTop(line) - parent->verticalScrollBar()->value());
            lineRect.moveLeft(0);
            lineRect.setRight(parent->width());
            painter->fillRect(lineRect, parent->colorScheme()->item(TextEditorColorScheme::ActiveLine).color());
            break;
        }
    }
    painter->restore();
}
