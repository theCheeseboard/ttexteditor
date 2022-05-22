#ifndef CARETRENDERSTEP_H
#define CARETRENDERSTEP_H

#include "texteditorrenderstep.h"

class CaretRenderStep : public TextEditorRenderStep {
        Q_OBJECT
    public:
        explicit CaretRenderStep(TextEditor* parent = nullptr);

    signals:

        // TextEditorRenderStep interface
    public:
        uint priority() const;
        void paint(QPainter* painter, QRect outputBounds, QRect redrawBounds);

        // TextEditorRenderStep interface
    public:
        RenderSide renderSide() const;
        int renderWidth() const;
        QString stepName() const;
};

#endif // CARETRENDERSTEP_H
