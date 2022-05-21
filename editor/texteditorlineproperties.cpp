#include "texteditor_p.h"

QMap<TextEditor::KnownLineProperty, QString> TextEditorPrivate::lineProperties = {
    {TextEditor::CompilationWarning, "CompilationWarning"},
    {TextEditor::CompilationError,   "CompilationError"  }
};

QList<TextEditor::KnownLineProperty> TextEditorPrivate::multiLineProperties = {
    TextEditor::CompilationWarning,
    TextEditor::CompilationError};
