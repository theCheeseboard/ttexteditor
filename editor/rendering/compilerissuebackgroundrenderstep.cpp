#include "compilerissuebackgroundrenderstep.h"

#include "libcontemporary_global.h"
#include "texteditor.h"
#include "texteditorcolorscheme.h"
#include <QPainter>
#include <QRect>
#include <QScrollBar>

CompilerIssueBackgroundRenderStep::CompilerIssueBackgroundRenderStep(TextEditor* parent) :
    TextEditorPerLineRenderStep{parent} {
}

TextEditorRenderStep::RenderSide CompilerIssueBackgroundRenderStep::renderSide() const {
    return Center;
}

int CompilerIssueBackgroundRenderStep::renderWidth() const {
    return 0;
}

QString CompilerIssueBackgroundRenderStep::stepName() const {
    return "CompilerIssueBackground";
}

uint CompilerIssueBackgroundRenderStep::priority() const {
    return LineActiveHighlight - 1;
}

void CompilerIssueBackgroundRenderStep::paintLine(int line, QPainter* painter, QRect outputBounds, QRect redrawBounds) {
    TextEditor* parent = this->parentEditor();

    QVariantList compilationErrors = parent->lineProperties(line, TextEditor::CompilationError);
    QVariantList compilationWarnings = parent->lineProperties(line, TextEditor::CompilationWarning);

    auto drawLineAnnotation = [=](QColor background) {
        QRect lineRect;
        lineRect.setHeight(parent->lineHeight(line));
        lineRect.moveTop(parent->lineTop(line) - parent->verticalScrollBar()->value());
        lineRect.moveLeft(0);
        lineRect.setRight(parent->width());
        painter->fillRect(lineRect, background);
    };

    if (!compilationErrors.isEmpty()) {
        drawLineAnnotation(parent->colorScheme()->item(TextEditorColorScheme::LineWithCompilationError).color());
    } else if (!compilationWarnings.isEmpty()) {
        drawLineAnnotation(parent->colorScheme()->item(TextEditorColorScheme::LineWithCompilationWarning).color());
    }
}
