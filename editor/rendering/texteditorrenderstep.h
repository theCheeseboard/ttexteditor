#ifndef TEXTEDITORRENDERSTEP_H
#define TEXTEDITORRENDERSTEP_H

#include <QObject>

class QPainter;
class TextEditor;
class QMouseEvent;
struct TextEditorRenderStepPrivate;
class TextEditorRenderStep : public QObject {
        Q_OBJECT
    public:
        explicit TextEditorRenderStep(TextEditor* parent = nullptr);
        ~TextEditorRenderStep();

        enum KnownRenderPriority : uint {
            EditorBackground = 0,
            LineActiveHighlight = 100,
            LeftGutterText = 200,
            LineText = 300,
            Carets = 400
        };

        TextEditor* parentEditor() const;

        enum RenderSide {
            Left,
            Right,
            Center,
            StackWithOther
        };

        virtual RenderSide renderSide() const = 0;
        virtual int renderWidth() const = 0;
        virtual QString renderStack() const;

        virtual QString stepName() const = 0;
        virtual uint priority() const;
        virtual void paint(QPainter* painter, QRect outputBounds, QRect redrawBounds) = 0;
        virtual bool mouseMoveEvent(QMouseEvent* event);

    signals:

    private:
        TextEditorRenderStepPrivate* d;
};

#endif // TEXTEDITORRENDERSTEP_H
