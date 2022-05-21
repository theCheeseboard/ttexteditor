#ifndef TEXTEDITORRENDERSTEP_H
#define TEXTEDITORRENDERSTEP_H

#include <QObject>

class QPainter;
class TextEditor;
struct TextEditorRenderStepPrivate;
class TextEditorRenderStep : public QObject {
        Q_OBJECT
    public:
        explicit TextEditorRenderStep(TextEditor* parent = nullptr);
        ~TextEditorRenderStep();

        TextEditor* parentEditor();

        virtual uint priority() const;
        virtual void paint(QPainter* painter) = 0;

    signals:

    private:
        TextEditorRenderStepPrivate* d;
};

#endif // TEXTEDITORRENDERSTEP_H
