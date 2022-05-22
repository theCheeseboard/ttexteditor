#include "caretrenderstep.h"

#include "textcaret.h"
#include "texteditor.h"
#include "texteditor_p.h"
#include <QRect>

CaretRenderStep::CaretRenderStep(TextEditor* parent) :
    TextEditorRenderStep{parent} {
}

uint CaretRenderStep::priority() const {
    return Carets;
}

void CaretRenderStep::paint(QPainter* painter, QRect outputBounds, QRect redrawBounds) {
    for (TextCaret* caret : parentEditor()->d->carets) {
        caret->drawCaret(painter);
    }
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
