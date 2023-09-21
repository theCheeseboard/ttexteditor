#ifndef COMPLETETEXTEDITOR_H
#define COMPLETETEXTEDITOR_H

#include "ttexteditor_global.h"
#include <QUrl>
#include <QWidget>

namespace Ui {
    class CompleteTextEditor;
}

class TextEditor;
class QListWidgetItem;
struct CompleteTextEditorPrivate;
class TTEXTEDITOR_EXPORT CompleteTextEditor : public QWidget {
        Q_OBJECT

    public:
        explicit CompleteTextEditor(QWidget* parent = nullptr);
        ~CompleteTextEditor();

        TextEditor* editor();

    private slots:
        void on_highlightingDefinitionSelector_clicked();

        void on_titleLabel_backButtonClicked();

        void on_highlightingDefinitionList_itemActivated(QListWidgetItem* item);

        void on_lineEdit_textEdited(const QString& arg1);

        void on_lineEdit_returnPressed();

    private:
        Ui::CompleteTextEditor* ui;
        CompleteTextEditorPrivate* d;

        void updateLineEndings();
        void updateHighlightingDefinition();
        void updateHighlightingDefinitionList();
};

#endif // COMPLETETEXTEDITOR_H
