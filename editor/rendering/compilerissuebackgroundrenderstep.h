#ifndef COMPILERISSUEBACKGROUNDRENDERSTEP_H
#define COMPILERISSUEBACKGROUNDRENDERSTEP_H

#include "texteditorrenderstep.h"

class CompilerIssueBackgroundRenderStep : public TextEditorRenderStep {
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
        void paint(QPainter* painter, QRect outputBounds, QRect redrawBounds);
};

#endif // COMPILERISSUEBACKGROUNDRENDERSTEP_H
