#include "completetexteditor.h"
#include "ui_completetexteditor.h"

#include "texteditorsyntaxhighlighter.h"
#include <KSyntaxHighlighting/definition.h>

struct CompleteTextEditorPrivate {
        QString query;
        QList<KSyntaxHighlighting::Definition> definitions;
};

CompleteTextEditor::CompleteTextEditor(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::CompleteTextEditor) {
    ui->setupUi(this);
    d = new CompleteTextEditorPrivate();

    d->definitions = ui->editor->highlighter()->repo()->definitions();
    std::sort(d->definitions.begin(), d->definitions.end(), [](const KSyntaxHighlighting::Definition& first, const KSyntaxHighlighting::Definition& second) {
        return first.translatedName().localeAwareCompare(second.translatedName());
    });
    updateHighlightingDefinitionList();

    connect(ui->editor, &TextEditor::lineEndingTypeChanged, this, &CompleteTextEditor::updateLineEndings);
    connect(ui->editor, &TextEditor::highlightingDefinitionChanged, this, &CompleteTextEditor::updateHighlightingDefinition);
    updateLineEndings();
    updateHighlightingDefinition();

    ui->stackedWidget->setCurrentWidget(ui->editorPage, false);
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);
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

void CompleteTextEditor::updateHighlightingDefinition() {
    ui->highlightingDefinitionSelector->setText(ui->editor->highlighter()->definition().name());
}

void CompleteTextEditor::updateHighlightingDefinitionList() {
    ui->highlightingDefinitionList->clear();
    for (const auto& def : d->definitions) {
        if (!d->query.isEmpty() && !def.translatedName().contains(d->query, Qt::CaseInsensitive)) continue;
        auto item = new QListWidgetItem();
        item->setText(def.translatedName());
        item->setData(Qt::UserRole, def.name());
        ui->highlightingDefinitionList->addItem(item);
    }
}

void CompleteTextEditor::on_highlightingDefinitionSelector_clicked() {
    ui->stackedWidget->setCurrentWidget(ui->highlightDefinitionSelectPage);
}

void CompleteTextEditor::on_titleLabel_backButtonClicked() {
    ui->stackedWidget->setCurrentWidget(ui->editorPage);
}

void CompleteTextEditor::on_highlightingDefinitionList_itemActivated(QListWidgetItem* item) {
    ui->editor->highlighter()->setDefinition(ui->editor->highlighter()->repo()->definitionForName(item->data(Qt::UserRole).toString()));
    ui->stackedWidget->setCurrentWidget(ui->editorPage);
}

void CompleteTextEditor::on_lineEdit_textEdited(const QString& arg1) {
    d->query = arg1;
    this->updateHighlightingDefinitionList();
}

void CompleteTextEditor::on_lineEdit_returnPressed() {
    if (ui->highlightingDefinitionList->count() > 0) {
        on_highlightingDefinitionList_itemActivated(ui->highlightingDefinitionList->item(0));
    }
}
