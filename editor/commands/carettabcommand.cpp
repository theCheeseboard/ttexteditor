#include "carettabcommand.h"

#include "texteditor.h"
#include "texteditor_p.h"

CaretTabCommand::CaretTabCommand(TextEditor* editor) :
    TextEditorCommand(editor) {
    auto preferSpaces = editor->preferSpaces();
    auto tabLength = editor->tabLength();

    for (int i = 0; i < editor->d->carets.length(); i++) {
        TextCaret* caret = editor->d->carets.at(i);

        if (caret->firstAnchor().y() == caret->lastAnchor().y()) {
            if (preferSpaces) {
                // Tab to the next tab stop
                this->pushEditorCommand({i,
                    QString(tabLength - (((caret->linePos().x() + 1) % tabLength) - 1), ' '),
                    true,
                    false});
            } else {
                // Push a tab
                this->pushEditorCommand({i,
                    "\t",
                    true,
                    false});
            }
        } else {
            // Tab all lines
            QString text;
            if (preferSpaces) {
                text = QString(tabLength, ' ');
            } else {
                text = "\t";
            }

            for (int i = caret->firstAnchor().y(); i <= caret->lastAnchor().y(); i++) {
                //                this->pushEditorCommand({i,
                //                    text,
                //                    true,
                //                    false});
            }
        }
    }
}

CaretTabCommand::~CaretTabCommand() {
}

int CaretTabCommand::id() const {
    return 3;
}
