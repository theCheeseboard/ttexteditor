#include "compilerissuebackgroundrenderstep.h"

#include "libcontemporary_global.h"
#include "texteditor.h"
#include "texteditorcolorscheme.h"
#include <QPainter>
#include <QRect>
#include <QScrollBar>

CompilerIssueBackgroundRenderStep::CompilerIssueBackgroundRenderStep(TextEditor* parent) :
    TextEditorRenderStep{parent} {
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

void CompilerIssueBackgroundRenderStep::paint(QPainter* painter, QRect outputBounds, QRect redrawBounds) {
    TextEditor* parent = this->parentEditor();

    painter->save();

    int firstLine = parent->lineAtY(redrawBounds.top());
    if (firstLine == -1) firstLine = parent->firstLineOnScreen();
    int lastLine = parent->lineAtY(redrawBounds.bottom());
    if (lastLine == -1) lastLine = parent->lastLineOnScreen();
    for (int i = firstLine; i <= lastLine; i++) {
        QVariantList compilationErrors = parent->lineProperties(i, TextEditor::CompilationError);
        QVariantList compilationWarnings = parent->lineProperties(i, TextEditor::CompilationWarning);

        auto drawLineAnnotation = [=](QColor background) {
            QRect lineRect;
            lineRect.setHeight(parent->lineHeight(i));
            lineRect.moveTop(parent->lineTop(i) - parent->verticalScrollBar()->value());
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

    painter->restore();
}
