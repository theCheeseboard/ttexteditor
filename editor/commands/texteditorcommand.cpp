#include "texteditorcommand.h"

#include "../texteditor.h"
#include "../texteditor_p.h"

struct TextEditorCommandPrivate {
        TextEditor* editor;

        SavedCarets initialCarets;
        SavedCarets lastCarets;

        QList<TextEditorCommand::EditorCommand> commands;
};

TextEditorCommand::TextEditorCommand(TextEditor* editor) {
    d = new TextEditorCommandPrivate();
    d->editor = editor;
    d->initialCarets = d->editor->d->saveCarets();
}

TextEditorCommand::~TextEditorCommand() {
    delete d;
}

void TextEditorCommand::pushEditorCommand(EditorCommand command) {
    d->commands.append(command);
}

void TextEditorCommand::pushCaretSelectionEraseCommand(int caret) {
    TextCaret* c = d->editor->d->carets.at(caret);
    QString textInAnchor;
    for (int i = d->editor->linePosToChar(c->firstAnchor()); i < d->editor->linePosToChar(c->lastAnchor()); i++) {
        QPoint linePos = d->editor->charToLinePos(i);
        QString lineContents = d->editor->d->lines.at(linePos.y())->contents;
        if (linePos.x() == lineContents.length()) {
            textInAnchor += "\n";
        } else {
            textInAnchor += lineContents.at(linePos.x());
        }
    }

    this->pushEditorCommand({caret, textInAnchor, false, false});
}

void TextEditorCommand::undo() {
    d->editor->d->loadCarets(d->lastCarets);
    for (auto command = d->commands.crbegin(); command != d->commands.crend(); command++) {
        TextCaret* caret = d->editor->d->carets.at(command->caret);
        if (command->isInsert) {
            for (int i = 0; i < command->text.length(); i++) {
                if (command->insertAfter) caret->moveCaretRelative(0, 1);
                caret->backspace();
            }
        } else {
            caret->insertText(command->text);
            if (command->insertAfter) caret->moveCaretRelative(0, -command->text.length());
        }
    }
    d->editor->d->loadCarets(d->initialCarets);
}

void TextEditorCommand::redo() {
    d->editor->d->loadCarets(d->initialCarets);
    for (auto command = d->commands.cbegin(); command != d->commands.cend(); command++) {
        TextCaret* caret = d->editor->d->carets.at(command->caret);
        if (command->isInsert) {
            caret->insertText(command->text);
            if (command->insertAfter) caret->moveCaretRelative(0, -command->text.length());
        } else {
            for (int i = 0; i < command->text.length(); i++) {
                if (command->insertAfter) caret->moveCaretRelative(0, 1);
                caret->backspace();
            }
        }
    }

    if (d->lastCarets.isEmpty()) d->lastCarets = d->editor->d->saveCarets();
}

bool TextEditorCommand::mergeWith(const QUndoCommand* other) {
    if (other->id() != this->id()) return false;

    const TextEditorCommand* otherCommand = static_cast<const TextEditorCommand*>(other);
    if (otherCommand->d->lastCarets.length() != this->d->lastCarets.length()) return false;

    SavedCarets carets = d->lastCarets;
    for (const EditorCommand& command : otherCommand->d->commands) {
        if (!command.insertAfter) {
            carets[command.caret].pos += command.isInsert ? command.text.length() : -command.text.length();
        }
    }
    for (int i = 0; i < this->d->lastCarets.length(); i++) {
        TextCaret::SavedCaret saved = otherCommand->d->lastCarets.at(i);
        TextCaret::SavedCaret thisSaved = carets.at(i);
        if (saved.line != thisSaved.line) return false;
        if (saved.pos != thisSaved.pos) return false;
    }

    this->d->commands.append(otherCommand->d->commands);
    this->d->lastCarets = otherCommand->d->lastCarets;
    return true;
}
