#include "texteditor_p.h"

const QMap<TextEditor::KnownLineProperty, QString> TextEditorPrivate::lineProperties = {
    {TextEditor::CompilationWarning, QStringLiteral("CompilationWarning")},
    {TextEditor::CompilationError,   QStringLiteral("CompilationError")  },
    {TextEditor::HighlightFormat,    QStringLiteral("HighlightFormat")   },
    {TextEditor::HighlightState,     QStringLiteral("HighlightState")    },
};

const QList<TextEditor::KnownLineProperty> TextEditorPrivate::multiLineProperties = {
    TextEditor::CompilationWarning,
    TextEditor::CompilationError,
    TextEditor::HighlightFormat};
