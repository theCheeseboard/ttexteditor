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

        void updateExpandedWidth();
        int errorColWidth() const;

        // TextEditorRenderStep interface
    public:
        void paint(QPainter* painter, QRect outputBounds, QRect redrawBounds);
        bool mouseMoveEvent(QMouseEvent* event);
        RenderSide renderSide() const;
        int renderWidth() const;
        QString stepName() const;
};

#endif // COMPILERISSUERENDERSTEP_H
