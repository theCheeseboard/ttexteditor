#ifndef LINETEXTRENDERSTEP_H
#define LINETEXTRENDERSTEP_H

#include "texteditorperlinerenderstep.h"
#include "ttexteditor_global.h"

class TTEXTEDITOR_EXPORT LineTextRenderStep : public TextEditorPerLineRenderStep {
        Q_OBJECT
    public:
        explicit LineTextRenderStep(TextEditor* parent = nullptr);

    signals:

    private:
        // TextEditorRenderStep interface
    public:
        uint priority() const;
        RenderSide renderSide() const;
        int renderWidth() const;
        QString stepName() const;

        // TextEditorPerLineRenderStep interface
    public:
        void paintLine(int line, QPainter* painter, QRect outputBounds, QRect redrawBounds);
};

#endif // LINETEXTRENDERSTEP_H
