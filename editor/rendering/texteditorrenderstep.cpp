#include "texteditorrenderstep.h"

#include "../texteditor.h"

struct TextEditorRenderStepPrivate {
        TextEditor* editor;
};

TextEditorRenderStep::TextEditorRenderStep(TextEditor* parent) :
    QObject{parent} {
    d = new TextEditorRenderStepPrivate();
    d->editor = parent;
}

TextEditorRenderStep::~TextEditorRenderStep() {
    delete d;
}

TextEditor* TextEditorRenderStep::parentEditor() {
    return d->editor;
}

uint TextEditorRenderStep::priority() const {
    return 1000;
}

bool TextEditorRenderStep::mouseMoveEvent(QMouseEvent* event) {
    return false;
}
