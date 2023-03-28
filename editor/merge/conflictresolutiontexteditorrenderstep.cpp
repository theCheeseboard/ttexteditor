#include "conflictresolutiontexteditorrenderstep.h"

#include <QPainter>
#include <QRect>
#include <QScrollBar>
#include <QVariant>
#include <texteditor.h>

struct ConflictResolutionTextEditorRenderStepPrivate {
};

ConflictResolutionTextEditorRenderStep::ConflictResolutionTextEditorRenderStep(TextEditor* parent) :
    TextEditorPerLineRenderStep{parent} {
    d = new ConflictResolutionTextEditorRenderStepPrivate();
}

ConflictResolutionTextEditorRenderStep::~ConflictResolutionTextEditorRenderStep() {
    delete d;
}

TextEditorRenderStep::RenderSide ConflictResolutionTextEditorRenderStep::renderSide() const {
    return Center;
}

int ConflictResolutionTextEditorRenderStep::renderWidth() const {
    return 0;
}

QString ConflictResolutionTextEditorRenderStep::stepName() const {
    return "ConflictResolution";
}

uint ConflictResolutionTextEditorRenderStep::priority() const {
    return LineActiveHighlight - 1;
}

void ConflictResolutionTextEditorRenderStep::paintLine(int line, QPainter* painter, QRect outputBounds, QRect redrawBounds) {
    auto* editor = parentEditor();

    painter->save();

    auto resolutions = editor->lineProperties(line, "conflictResolution");
    if (resolutions.isEmpty()) return;

    auto resolutionType = resolutions.first().toInt();

    int resolutionStage = resolutionType % 5;
    int resolutionBase = resolutionType / 5;

    QColor background;
    QColor border;

    switch (resolutionBase) {
        case 0:
            background = QColor(200, 0, 0, 100);
            border = Qt::red;
            break;
        case 1:
            background = QColor(127, 127, 127, 100);
            border = editor->palette().color(QPalette::WindowText);
            break;
    }

    QRect lineRect;
    lineRect.setHeight(editor->lineHeight(line));
    lineRect.moveTop(editor->lineTop(line) - editor->verticalScrollBar()->value());
    lineRect.moveLeft(0);
    lineRect.setRight(editor->width());

    if (resolutionStage != 4) painter->fillRect(lineRect, background);
    painter->setPen(border);

    if (resolutionStage == 0 || resolutionStage == 3 || resolutionStage == 4) {
        // Requires top border
        painter->drawLine(lineRect.topLeft(), lineRect.topRight());
    }
    if (resolutionStage == 1 || resolutionStage == 3) {
        // Requires bottom border
        painter->drawLine(lineRect.bottomLeft(), lineRect.bottomRight());
    }

    painter->restore();
}
