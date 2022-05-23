#ifndef LINETEXTRENDERSTEP_H
#define LINETEXTRENDERSTEP_H

#include "texteditorrenderstep.h"

class LineTextRenderStep : public TextEditorRenderStep {
        Q_OBJECT
    public:
        explicit LineTextRenderStep(TextEditor* parent = nullptr);

    signals:

    private:
        void drawLine(int line, QRect outputBounds, QPainter* painter);

        // TextEditorRenderStep interface
    public:
        uint priority() const;
        void paint(QPainter* painter, QRect outputBounds, QRect redrawBounds);
        RenderSide renderSide() const;
        int renderWidth() const;
        QString stepName() const;
};

#endif // LINETEXTRENDERSTEP_H
