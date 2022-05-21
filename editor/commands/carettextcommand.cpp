#include "carettextcommand.h"

#include "../textcaret.h"
#include "../texteditor.h"
#include "../texteditor_p.h"

CaretTextCommand::CaretTextCommand(TextEditor* editor, QString text) :
    TextEditorCommand(editor) {
    for (int i = 0; i < editor->d->carets.length(); i++) {
        TextCaret* caret = editor->d->carets.at(i);

        if (caret->firstAnchor() != caret->lastAnchor()) {
            // Erase the selection first
            this->pushCaretSelectionEraseCommand(i);
        }

        this->pushEditorCommand({i,
            text,
            true,
            false});
    }
}

CaretTextCommand::~CaretTextCommand() {
}

int CaretTextCommand::id() const {
    return 1;
}
