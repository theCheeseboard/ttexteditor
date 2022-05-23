#include "activelinebackgroundrenderstep.h"

#include "textcaret.h"
#include "texteditor.h"
#include "texteditor_p.h"
#include "texteditorcolorscheme.h"
#include <QPainter>
#include <QRect>

ActiveLineBackgroundRenderStep::ActiveLineBackgroundRenderStep(TextEditor* parent) :
    TextEditorRenderStep{parent} {
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

void ActiveLineBackgroundRenderStep::paint(QPainter* painter, QRect outputBounds, QRect redrawBounds) {
    TextEditor* parent = parentEditor();

    painter->save();

    QPolygon activeLinesPolygon;

    for (TextCaret* caret : parent->d->carets) {
        if (caret->firstAnchor() == caret->lastAnchor()) {
            QRect lineRect = caret->caretRect();
            lineRect.moveTop(lineRect.top() - parent->verticalScrollBar()->value());
            lineRect.moveLeft(0);
            lineRect.setRight(parent->width());
            activeLinesPolygon = activeLinesPolygon.united(lineRect);
        }
    }

    painter->setPen(Qt::transparent);
    painter->setBrush(parent->colorScheme()->item(TextEditorColorScheme::ActiveLine));
    painter->drawPolygon(activeLinesPolygon);

    painter->restore();
}
