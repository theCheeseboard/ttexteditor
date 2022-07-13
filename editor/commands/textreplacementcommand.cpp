#include "textreplacementcommand.h"

#include "texteditor.h"
#include "texteditor_p.h"
#include <QPoint>

struct TextReplacementCommandPrivate {
        TextEditor* editor;

        QPoint anchorStart;
        QPoint originalAnchorEnd;
        QPoint replacementAnchorEnd;

        QString originalText;
        QString replacementText;
};

TextReplacementCommand::TextReplacementCommand(TextEditor* editor, QPoint anchorStart, QPoint anchorEnd, QString replacement) :
    QUndoCommand() {
    d = new TextReplacementCommandPrivate();
    d->editor = editor;
    d->anchorStart = anchorStart;
    d->originalAnchorEnd = anchorEnd;
    d->replacementText = replacement;

    // Get the original text
    QString text;
    for (auto c = d->editor->linePosToChar(anchorStart); c != d->editor->linePosToChar(anchorEnd); c++) {
        auto pos = d->editor->charToLinePos(c);
        auto lineContents = d->editor->d->lines.at(pos.y())->contents;
        if (pos.x() == lineContents.length()) {
            text.append("\n");
        } else {
            text.append(lineContents.at(pos.x()));
        }
    }
    d->originalText = text;

    // Get the anchor end
    auto newEnd = anchorStart;
    for (auto c : qAsConst(replacement)) {
        if (replacement == "\n") {
            newEnd.ry()++;
            newEnd.setX(0);
        } else {
            newEnd.rx()++;
        }
    }
    d->replacementAnchorEnd = newEnd;
}

TextReplacementCommand::~TextReplacementCommand() {
    delete d;
}

void TextReplacementCommand::replace(QPoint anchorStart, QPoint anchorEnd, QString replacement) {
    for (auto* caret : d->editor->d->carets) {
        if (caret->isPrimary()) {
            caret->moveCaret(anchorEnd);
        } else {
            caret->discontinueAndDelete();
        }
    }

    QList<TextDelta> deltas;
    auto caret = d->editor->d->carets.first();
    int deletions = d->editor->linePosToChar(anchorEnd) - d->editor->linePosToChar(anchorStart);
    for (auto i = 0; i < deletions; i++) deltas.append(caret->backspace());
    deltas.append(caret->insertText(replacement));

    emit d->editor->textChanged(deltas);
}

void TextReplacementCommand::undo() {
    replace(d->anchorStart, d->replacementAnchorEnd, d->originalText);

    //    for (auto* caret : d->editor->d->carets) {
    //        if (caret->isPrimary()) {
    //            caret->moveCaret(d->originalAnchorEnd);
    //        } else {
    //            caret->discontinueAndDelete();
    //        }
    //    }
}

void TextReplacementCommand::redo() {
    replace(d->anchorStart, d->originalAnchorEnd, d->replacementText);

    //    for (auto* caret : d->editor->d->carets) {
    //        if (caret->isPrimary()) {
    //            caret->moveCaret(d->replacementAnchorEnd);
    //        } else {
    //            caret->discontinueAndDelete();
    //        }
    //    }
}

int TextReplacementCommand::id() const {
    return 4;
}
