#ifndef ACTIVELINEBACKGROUNDRENDERSTEP_H
#define ACTIVELINEBACKGROUNDRENDERSTEP_H

#include "texteditorrenderstep.h"

class ActiveLineBackgroundRenderStep : public TextEditorRenderStep {
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
        void paint(QPainter* painter, QRect outputBounds, QRect redrawBounds);
};

#endif // ACTIVELINEBACKGROUNDRENDERSTEP_H
