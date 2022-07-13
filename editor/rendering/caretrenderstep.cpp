#include "caretrenderstep.h"

#include "textcaret.h"
#include "texteditor.h"
#include "texteditor_p.h"
#include <QPainter>
#include <QRect>

CaretRenderStep::CaretRenderStep(TextEditor* parent) :
    TextEditorRenderStep{parent} {
}

uint CaretRenderStep::priority() const {
    return Carets;
}

void CaretRenderStep::paint(QPainter* painter, QRect outputBounds, QRect redrawBounds) {
    painter->save();
    //    painter->setClipRect(outputBounds);
    //    painter->setClipping(true);
    for (TextCaret* caret : parentEditor()->d->carets) {
        if (parentEditor()->readOnly() && caret->firstAnchor() == caret->lastAnchor()) continue;
        caret->drawCaret(painter);
    }
    painter->restore();
}

TextEditorRenderStep::RenderSide CaretRenderStep::renderSide() const {
    return Center;
}

int CaretRenderStep::renderWidth() const {
    return 0;
}

QString CaretRenderStep::stepName() const {
    return "Carets";
}
