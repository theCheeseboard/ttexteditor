#ifndef COMPILERISSUERENDERSTEP_H
#define COMPILERISSUERENDERSTEP_H

#include "texteditorperlinerenderstep.h"

struct CompilerIssueRenderStepPrivate;
class CompilerIssueRenderStep : public TextEditorPerLineRenderStep {
        Q_OBJECT
    public:
        explicit CompilerIssueRenderStep(TextEditor* parent = nullptr);
        ~CompilerIssueRenderStep();

    signals:

    private:
        CompilerIssueRenderStepPrivate* d;

        void updateExpandedWidth();
        int errorColWidth() const;

        // TextEditorRenderStep interface
    public:
        bool mouseMoveEvent(QMouseEvent* event);
        RenderSide renderSide() const;
        int renderWidth() const;
        QString stepName() const;

        // TextEditorPerLineRenderStep interface
    public:
        void paintLine(int line, QPainter* painter, QRect outputBounds, QRect redrawBounds);
};

#endif // COMPILERISSUERENDERSTEP_H
