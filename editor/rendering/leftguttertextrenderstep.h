#ifndef LEFTGUTTERTEXTRENDERSTEP_H
#define LEFTGUTTERTEXTRENDERSTEP_H

#include "texteditorperlinerenderstep.h"

class LeftGutterTextRenderStep : public TextEditorPerLineRenderStep {
        Q_OBJECT
    public:
        explicit LeftGutterTextRenderStep(TextEditor* parent = nullptr);

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

#endif // LEFTGUTTERTEXTRENDERSTEP_H
