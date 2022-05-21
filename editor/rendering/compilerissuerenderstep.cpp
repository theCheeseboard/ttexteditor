#include "compilerissuerenderstep.h"

#include <QPainter>
#include <tvariantanimation.h>

struct CompilerIssueRenderStepPrivate {
};

CompilerIssueRenderStep::CompilerIssueRenderStep(TextEditor* parent) :
    TextEditorRenderStep{parent} {
    d = new CompilerIssueRenderStepPrivate();
}

CompilerIssueRenderStep::~CompilerIssueRenderStep() {
    delete d;
}

void CompilerIssueRenderStep::paint(QPainter* painter, QRect redrawBounds) {
}
