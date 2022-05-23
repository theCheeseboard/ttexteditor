#ifndef TEXTEDITORPERLINERENDERSTEP_H
#define TEXTEDITORPERLINERENDERSTEP_H

#include "texteditorrenderstep.h"

class TextEditorPerLineRenderStep : public TextEditorRenderStep {
        Q_OBJECT
    public:
        explicit TextEditorPerLineRenderStep(TextEditor* parent = nullptr);

        virtual void paintLine(int line, QPainter* painter, QRect outputBounds, QRect redrawBounds) = 0;

    signals:

        // TextEditorRenderStep interface
    public:
        void paint(QPainter* painter, QRect outputBounds, QRect redrawBounds);
};

#endif // TEXTEDITORPERLINERENDERSTEP_H
