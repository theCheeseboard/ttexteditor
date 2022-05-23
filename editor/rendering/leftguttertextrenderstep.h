#ifndef LEFTGUTTERTEXTRENDERSTEP_H
#define LEFTGUTTERTEXTRENDERSTEP_H

#include "texteditorrenderstep.h"

class LeftGutterTextRenderStep : public TextEditorRenderStep {
        Q_OBJECT
    public:
        explicit LeftGutterTextRenderStep(TextEditor* parent = nullptr);

    signals:

    private:
        void drawLine(int line, QRect outputBounds, QPainter* painter);

        // TextEditorRenderStep interface
    public:
        RenderSide renderSide() const;
        int renderWidth() const;
        QString stepName() const;
        uint priority() const;
        void paint(QPainter* painter, QRect outputBounds, QRect redrawBounds);
};

#endif // LEFTGUTTERTEXTRENDERSTEP_H
