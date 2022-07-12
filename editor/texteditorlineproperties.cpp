#include "texteditor_p.h"

const QMap<TextEditor::KnownLineProperty, QString> TextEditorPrivate::lineProperties = {
    {TextEditor::CompilationWarning, QStringLiteral("CompilationWarning")},
    {TextEditor::CompilationError,   QStringLiteral("CompilationError")  }
};

const QList<TextEditor::KnownLineProperty> TextEditorPrivate::multiLineProperties = {
    TextEditor::CompilationWarning,
    TextEditor::CompilationError};
