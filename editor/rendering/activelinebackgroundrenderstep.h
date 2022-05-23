#ifndef ACTIVELINEBACKGROUNDRENDERSTEP_H
#define ACTIVELINEBACKGROUNDRENDERSTEP_H

#include "texteditorperlinerenderstep.h"

class ActiveLineBackgroundRenderStep : public TextEditorPerLineRenderStep {
        Q_OBJECT
    public:
        explicit ActiveLineBackgroundRenderStep(TextEditor* parent = nullptr);

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

#endif // ACTIVELINEBACKGROUNDRENDERSTEP_H
