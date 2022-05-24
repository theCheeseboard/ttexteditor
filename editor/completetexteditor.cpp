#include "completetexteditor.h"
#include "ui_completetexteditor.h"

struct CompleteTextEditorPrivate {
};

CompleteTextEditor::CompleteTextEditor(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::CompleteTextEditor) {
    ui->setupUi(this);
    d = new CompleteTextEditorPrivate();

    connect(ui->editor, &TextEditor::lineEndingTypeChanged, this, &CompleteTextEditor::updateLineEndings);
    updateLineEndings();
}

CompleteTextEditor::~CompleteTextEditor() {
    delete ui;
    delete d;
}

TextEditor* CompleteTextEditor::editor() {
    return ui->editor;
}

void CompleteTextEditor::updateLineEndings() {
    switch (ui->editor->currentLineEndingType()) {
        case TextEditor::UnixLineEndings:
            ui->lineEndingsSelector->setText(QStringLiteral("LF"));
            break;
        case TextEditor::WinLineEndings:
            ui->lineEndingsSelector->setText(QStringLiteral("CR LF"));
            break;
        case TextEditor::ClassicMacLineEndings:
            ui->lineEndingsSelector->setText(QStringLiteral("CR"));
            break;
    }
}
