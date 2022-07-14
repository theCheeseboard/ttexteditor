#ifndef CONFLICTRESOLUTIONTEXTEDITORRENDERSTEP_H
#define CONFLICTRESOLUTIONTEXTEDITORRENDERSTEP_H

#include <rendering/texteditorperlinerenderstep.h>

struct ConflictResolutionTextEditorRenderStepPrivate;
class ConflictResolutionTextEditorRenderStep : public TextEditorPerLineRenderStep {
        Q_OBJECT
    public:
        explicit ConflictResolutionTextEditorRenderStep(TextEditor* parent = nullptr);
        ~ConflictResolutionTextEditorRenderStep();

    signals:

    private:
        ConflictResolutionTextEditorRenderStepPrivate* d;

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

#endif // CONFLICTRESOLUTIONTEXTEDITORRENDERSTEP_H
