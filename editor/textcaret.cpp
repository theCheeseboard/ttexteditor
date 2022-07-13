#include "textcaret.h"

#include "texteditor.h"
#include "texteditor_p.h"
#include "texteditorcolorscheme.h"

#include <QKeyEvent>
#include <QPainter>
#include <QRegularExpression>
#include <tvariantanimation.h>

struct TextCaretPrivate {
        TextEditor* editor;
        int line;
        int pos;
        int anchor;

        bool isPrimary = false;
        bool discontinued = false;

        QPolygon anchorPoly;

        tVariantAnimation* anim;

        static tVariantAnimation* caretBlinkTimer;
        static QRegularExpression newLineSplit;
};

tVariantAnimation* TextCaretPrivate::caretBlinkTimer = nullptr;
QRegularExpression TextCaretPrivate::newLineSplit = QRegularExpression("\\r\\n|\\r|\\n");

TextCaret::TextCaret(int line, int pos, TextEditor* parent) :
    QObject{parent} {
    d = new TextCaretPrivate();
    d->editor = parent;
    d->anchor = 0;

    if (d->caretBlinkTimer == nullptr) {
        d->caretBlinkTimer = new tVariantAnimation();
        d->caretBlinkTimer->setStartValue(1.0);
        d->caretBlinkTimer->setKeyValueAt(0.5, 0.0);
        d->caretBlinkTimer->setEndValue(1.0);
        d->caretBlinkTimer->setDuration(1000);
        d->caretBlinkTimer->setForceAnimation(true);
        d->caretBlinkTimer->setLoopCount(-1);
        d->caretBlinkTimer->start();
    }

    connect(d->caretBlinkTimer, &tVariantAnimation::valueChanged, this, [=] {
        d->editor->update(d->anim->currentValue().toRect());
    });

    d->anim = new tVariantAnimation();
    d->anim->setDuration(50);
    d->anim->setEasingCurve(QEasingCurve::OutCubic);
    connect(d->anim, &tVariantAnimation::valueChanged, parent, QOverload<>::of(&TextEditor::update));

    moveCaret(line, pos);
    d->anim->stop();
    d->anim->setStartValue(d->anim->endValue());

    recalculateAnchor();
}

TextCaret::~TextCaret() {
    delete d;
}

TextCaret* TextCaret::fromSavedCaret(SavedCaret caret) {
    auto* c = new TextCaret(caret.line, caret.pos, caret.parent);
    c->loadCaret(caret);
    return c;
}

TextCaret::SavedCaret TextCaret::saveCaret() {
    SavedCaret saved;
    saved.parent = d->editor;
    saved.line = d->line;
    saved.pos = d->pos;
    saved.anchor = d->anchor;
    return saved;
}

void TextCaret::loadCaret(SavedCaret caret) {
    // Not a caret for this editor
    if (caret.parent != d->editor) return;
    d->line = caret.line;
    d->pos = caret.pos;
    d->anchor = caret.anchor;

    recalculateAnchor();
}

void TextCaret::moveCaret(int line, int pos) {
    d->line = line;
    d->pos = pos;
    d->anchor = 0;

    QString lineContents = d->editor->d->lines.at(line)->contents;

    QRect caretRect;
    caretRect.moveTop(d->editor->lineTop(line));
    caretRect.moveLeft(d->editor->renderStepOutputArea("LineText").left() + d->editor->fontMetrics().horizontalAdvance(lineContents.left(pos)));
    caretRect.setHeight(d->editor->lineHeight(line));
    caretRect.setWidth(SC_DPI_W(1, d->editor));
    moveCaret(caretRect);

    recalculateAnchor();
}

void TextCaret::moveCaret(QPoint linePos) {
    moveCaret(linePos.y(), linePos.x());
}

void TextCaret::moveCaretRelative(int lines, int cols) {
    int newLine = d->line + lines;

    if (newLine < 0) newLine = 0;
    if (newLine >= d->editor->d->lines.length()) newLine = d->editor->d->lines.length() - 1;

    auto newLinePos = QPoint(d->pos, newLine);
    newLinePos.setX(qMin(newLinePos.x(), d->editor->d->lines.at(newLinePos.y())->contents.length()));
    moveCaret(d->editor->charToLinePos(d->editor->linePosToChar(newLinePos) + cols));
}

void TextCaret::moveCaretToStartOfLine() {
    moveCaret(d->line, 0);
}

void TextCaret::moveCaretToEndOfLine() {
    moveCaret(d->line, d->editor->d->lines.at(d->line)->contents.length());
}

void TextCaret::setAnchor(int line, int pos) {
    int c = d->editor->linePosToChar(QPoint(d->pos, d->line));
    d->anchor = d->editor->linePosToChar(QPoint(pos, line)) - c;

    recalculateAnchor();
}

void TextCaret::setAnchor(QPoint linePos) {
    setAnchor(linePos.y(), linePos.x());
}

void TextCaret::moveAnchorRelative(int lines, int cols) {
    int c = d->editor->linePosToChar(QPoint(d->pos, d->line));
    QPoint anchorPos = d->editor->charToLinePos(c + d->anchor);
    int newAnchorC = d->editor->linePosToChar(anchorPos + QPoint(cols, lines));
    d->anchor = newAnchorC - c;

    recalculateAnchor();
}

QPoint TextCaret::firstAnchor() {
    if (d->anchor < 0) {
        return d->editor->charToLinePos(d->editor->linePosToChar(QPoint(d->pos, d->line)) + d->anchor);
    } else {
        return QPoint(d->pos, d->line);
    }
}

QPoint TextCaret::lastAnchor() {
    if (d->anchor > 0) {
        return d->editor->charToLinePos(d->editor->linePosToChar(QPoint(d->pos, d->line)) + d->anchor);
    } else {
        return QPoint(d->pos, d->line);
    }
}

QString TextCaret::textBetweenAnchors() {
    QString str;
    for (int c = d->editor->linePosToChar(this->firstAnchor()); c < d->editor->linePosToChar(this->lastAnchor()); c++) {
        auto linePos = d->editor->charToLinePos(c);
        auto contents = d->editor->d->lines.at(linePos.y())->contents;
        if (linePos.x() == contents.length()) {
            str += d->editor->lineEndingString();
        } else {
            str += contents.at(linePos.x());
        }
    }
    return str;
}

void TextCaret::drawCaret(QPainter* painter) {
    painter->save();
    QRect rect = d->anim->currentValue().toRect();
    rect.moveTopLeft(rect.topLeft() - QPoint(d->editor->d->hScrollBar->value(), d->editor->d->vScrollBar->value()));

    painter->setOpacity(d->caretBlinkTimer->currentValue().toReal());
    painter->fillRect(rect, d->editor->colorScheme()->item(TextEditorColorScheme::NormalText));
    painter->restore();

    if (d->anchor != 0) {
        painter->save();
        // Draw the anchor
        auto offset = d->editor->characterRect(this->firstAnchor()).topLeft();
        painter->setBrush(d->editor->colorScheme()->item(TextEditorColorScheme::HighlightedText));
        painter->setPen(d->editor->colorScheme()->item(TextEditorColorScheme::HighlightedTextBorder).color());
        painter->drawPolygon(d->anchorPoly.translated(offset));
        painter->restore();
    }
}

void TextCaret::setIsPrimary(bool primary) {
    d->isPrimary = primary;
}

bool TextCaret::isPrimary() {
    return d->isPrimary;
}

TextDelta TextCaret::insertText(QString text) {
    TextDelta delta;
    delta.replacement = text;
    delta.startEdit = this->firstAnchor();
    delta.endEdit = this->lastAnchor();

    // Mutate the text editor's text and update the carets
    TextEditorPrivate::Line* line = d->editor->d->lines.at(d->line);

    QStringList splitLines = text.split(d->newLineSplit);
    QString firstLineText = splitLines.takeFirst();

    line->contents = line->contents.insert(d->pos, firstLineText);

    // Move each affected caret
    for (TextCaret* caret : d->editor->d->carets) {
        if (caret->d->line == d->line && caret->d->pos >= d->pos) {
            caret->moveCaret(caret->d->line, caret->d->pos + firstLineText.length());
        }
    }

    // Remove the rest of the line while handing new lines
    QString restOfLine = line->contents.mid(d->pos);
    line->contents.truncate(d->pos);

    // Handle new lines
    while (!splitLines.isEmpty()) {
        auto* newLine = new TextEditorPrivate::Line();
        newLine->contents = splitLines.takeFirst();
        d->editor->d->lines.insert(d->line + 1, newLine);
        d->editor->repositionElements();

        for (TextCaret* caret : d->editor->d->carets) {
            if (caret->d->line > d->line) {
                caret->moveCaret(caret->d->line + 1, caret->d->pos);
            }
        }
        this->moveCaret(d->line + 1, newLine->contents.length());
    }

    d->editor->d->lines.at(d->line)->contents.append(restOfLine);

    return delta;
}

TextDelta TextCaret::backspace() {
    if (d->pos == 0) {
        if (d->line == 0) { // Can't backspace behind first line
            TextDelta delta;
            delta.startEdit = this->firstAnchor();
            delta.endEdit = this->lastAnchor();
            delta.replacement = "";
            return delta;
        }

        TextDelta delta;
        delta.replacement = "";
        delta.endEdit = this->linePos();

        TextEditorPrivate::Line* lineToRemove = d->editor->d->lines.at(d->line);
        TextEditorPrivate::Line* lineToCombineTo = d->editor->d->lines.at(d->line - 1);
        int newCaretPos = lineToCombineTo->contents.length();
        lineToCombineTo->contents += lineToRemove->contents;

        int currentLine = d->line;
        for (TextCaret* caret : d->editor->d->carets) {
            if (caret->d->line > currentLine) {
                caret->moveCaret(caret->d->line - 1, caret->d->pos);
            } else if (caret->d->line == currentLine) {
                caret->moveCaret(caret->d->line - 1, newCaretPos + caret->d->pos);
            }
        }

        d->editor->d->lines.removeAll(lineToRemove);
        d->editor->repositionElements();

        delta.startEdit = this->linePos();
        return delta;
    } else {
        TextDelta delta;
        delta.replacement = "";
        delta.endEdit = this->linePos();

        TextEditorPrivate::Line* line = d->editor->d->lines.at(d->line);
        line->contents.remove(d->pos - 1, 1);

        for (TextCaret* caret : d->editor->d->carets) {
            if (caret->d->line == d->line && caret->d->pos >= d->pos) {
                caret->moveCaret(caret->d->line, caret->d->pos - 1);
            }
        }

        delta.startEdit = this->linePos();
        return delta;
    }
}

QPoint TextCaret::linePos() {
    return QPoint(d->pos, d->line);
}

QRect TextCaret::caretRect() {
    return d->anim->currentValue().toRect();
}

void TextCaret::discontinueAndDelete() {
    if (d->discontinued) return;

    emit discontinued();
    this->deleteLater();
    d->discontinued = true;
}

bool TextCaret::isDiscontinued() {
    return d->discontinued;
}

void TextCaret::moveCaret(QRect newPos) {
    d->anim->stop();
    d->anim->setStartValue(d->anim->currentValue());
    d->anim->setEndValue(newPos);
    d->anim->start();

    d->caretBlinkTimer->stop();
    d->caretBlinkTimer->setCurrentTime(0);
    d->caretBlinkTimer->start();

    d->editor->simplifyCarets();
}

void TextCaret::recalculateAnchor()
{
    //TODO: Parallelism???
    QPolygon anchorPoly;
    int firstAnchor = d->editor->linePosToChar(this->firstAnchor());
    int lastAnchor = d->editor->linePosToChar(this->lastAnchor());
    auto offset = d->editor->characterRect(this->firstAnchor()).topLeft();
    for (int i = firstAnchor; i < lastAnchor; i++) {
        anchorPoly = anchorPoly.united(d->editor->characterRect(d->editor->charToLinePos(i)));
    }
    d->anchorPoly = anchorPoly.translated(-offset);
}

bool TextCaret::eventFilter(QObject* watched, QEvent* event) {
    if (watched == d->editor) {
        if (event->type() == QEvent::KeyPress) {
            auto* keyEvent = static_cast<QKeyEvent*>(event);
            Qt::KeyboardModifiers modifiers = keyEvent->modifiers() & ~Qt::KeypadModifier;

            if (modifiers == Qt::NoModifier || modifiers == Qt::ShiftModifier) {
                if (keyEvent->key() == Qt::Key_Backspace) {
                    backspace();
                } else if (!keyEvent->text().isEmpty()) {
                    insertText(keyEvent->text());
                }
            }

            if (modifiers == Qt::NoModifier) {
                if (keyEvent->key() == Qt::Key_Up) {
                    moveCaretRelative(-1, 0);
                } else if (keyEvent->key() == Qt::Key_Down) {
                    moveCaretRelative(1, 0);
                } else if (keyEvent->key() == Qt::Key_Left) {
                    moveCaretRelative(0, -1);
                } else if (keyEvent->key() == Qt::Key_Right) {
                    moveCaretRelative(0, 1);
                }
            }

            d->editor->update();
            return false;
        }
    }
    return QObject::eventFilter(watched, event);
}
