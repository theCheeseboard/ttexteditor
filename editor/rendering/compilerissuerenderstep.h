#ifndef COMPILERISSUERENDERSTEP_H
#define COMPILERISSUERENDERSTEP_H

#include "texteditorrenderstep.h"

struct CompilerIssueRenderStepPrivate;
class CompilerIssueRenderStep : public TextEditorRenderStep {
        Q_OBJECT
    public:
        explicit CompilerIssueRenderStep(TextEditor* parent = nullptr);
        ~CompilerIssueRenderStep();

    signals:

    private:
        CompilerIssueRenderStepPrivate* d;

        // TextEditorRenderStep interface
    public:
        void paint(QPainter* painter, QRect redrawBounds);
};

#endif // COMPILERISSUERENDERSTEP_H
