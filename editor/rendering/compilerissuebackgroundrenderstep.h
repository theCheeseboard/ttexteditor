#ifndef COMPILERISSUEBACKGROUNDRENDERSTEP_H
#define COMPILERISSUEBACKGROUNDRENDERSTEP_H

#include "texteditorperlinerenderstep.h"

class CompilerIssueBackgroundRenderStep : public TextEditorPerLineRenderStep {
        Q_OBJECT
    public:
        explicit CompilerIssueBackgroundRenderStep(TextEditor* parent = nullptr);

    signals:

        // TextEditorRenderStep interface
    public:
        RenderSide renderSide() const;
        int renderWidth() const;
        QString stepName() const;
        uint priority() const;

        // TextEditorPerLineRenderStep interface
    public:
        void paintLine(int line, QPainter* painter, QRect outputBounds, QRect redrawBounds);
};

#endif // COMPILERISSUEBACKGROUNDRENDERSTEP_H
