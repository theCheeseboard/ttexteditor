#ifndef COMPILERISSUERENDERSTEP_H
#define COMPILERISSUERENDERSTEP_H

#include "texteditorrenderstep.h"

class CompilerIssueRenderStep : public TextEditorRenderStep {
        Q_OBJECT
    public:
        explicit CompilerIssueRenderStep(TextEditor* parent = nullptr);

    signals:

        // TextEditorRenderStep interface
    public:
        void paint(QPainter* painter);
};

#endif // COMPILERISSUERENDERSTEP_H
