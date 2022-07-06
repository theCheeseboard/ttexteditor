#ifndef COMPLETETEXTEDITOR_H
#define COMPLETETEXTEDITOR_H

#include "ttexteditor_global.h"
#include <QUrl>
#include <QWidget>

namespace Ui {
    class CompleteTextEditor;
}

class TextEditor;
struct CompleteTextEditorPrivate;
class TTEXTEDITOR_EXPORT CompleteTextEditor : public QWidget {
        Q_OBJECT

    public:
        explicit CompleteTextEditor(QWidget* parent = nullptr);
        ~CompleteTextEditor();

        TextEditor* editor();

    private:
        Ui::CompleteTextEditor* ui;
        CompleteTextEditorPrivate* d;

        void updateLineEndings();
};

#endif // COMPLETETEXTEDITOR_H
