#ifndef TEXTEDITORRENDERSTEP_H
#define TEXTEDITORRENDERSTEP_H

#include <QObject>

class QPainter;
class TextEditor;
class TextEditorRenderStep : public QObject {
        Q_OBJECT
    public:
        explicit TextEditorRenderStep(TextEditor* parent = nullptr);

        virtual void paint(QPainter* painter) = 0;

    signals:
};

#endif // TEXTEDITORRENDERSTEP_H
