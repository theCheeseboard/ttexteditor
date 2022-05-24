﻿#include "texteditor.h"

#include "textcaret.h"
#include "texteditorcolorscheme.h"
#include <QClipboard>
#include <QFontDatabase>
#include <QMenu>
#include <QPainter>
#include <QRandomGenerator64>
#include <QScrollBar>
#include <QWheelEvent>
#include <libcontemporary_global.h>

#include "commands/careterasecommand.h"
#include "commands/carettextcommand.h"

#include "rendering/activelinebackgroundrenderstep.h"
#include "rendering/caretrenderstep.h"
#include "rendering/compilerissuebackgroundrenderstep.h"
#include "rendering/compilerissuerenderstep.h"
#include "rendering/leftguttertextrenderstep.h"
#include "rendering/linetextrenderstep.h"

#include "texteditor_p.h"

TextEditor::TextEditor(QWidget* parent) :
    QWidget{parent} {
    d = new TextEditorPrivate();

    this->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    this->setMouseTracking(true);
    this->setFocusPolicy(Qt::StrongFocus);

    d->lines.append(new TextEditorPrivate::Line{"Line 1 Text"});
    d->lines.append(new TextEditorPrivate::Line{"Line 2 Text"});
    d->lines.append(new TextEditorPrivate::Line{"Line 3 Text"});
    d->lines.append(new TextEditorPrivate::Line{"Line 4 Text"});
    d->lines.append(new TextEditorPrivate::Line{"Line 5 Text"});
    d->lines.append(new TextEditorPrivate::Line{""});
    d->undoStack = new QUndoStack(this);
    connect(d->undoStack, &QUndoStack::cleanChanged, this, &TextEditor::unsavedChangesChanged);

    d->vScrollBar = new QScrollBar(Qt::Vertical);
    d->vScrollBar->setFixedWidth(this->style()->pixelMetric(QStyle::PM_ScrollBarExtent));
    d->vScrollBar->setParent(this);
    connect(d->vScrollBar, &QScrollBar::valueChanged, this, QOverload<>::of(&TextEditor::repaint));
    d->vScrollBar->show();

    d->hScrollBar = new QScrollBar(Qt::Horizontal);
    d->hScrollBar->setFixedHeight(this->style()->pixelMetric(QStyle::PM_ScrollBarExtent));
    d->hScrollBar->setParent(this);
    connect(d->hScrollBar, &QScrollBar::valueChanged, this, QOverload<>::of(&TextEditor::repaint));
    d->hScrollBar->show();

    addCaret(0, 0);
    d->carets.first()->setIsPrimary(true);

    d->colorScheme = new TextEditorColorScheme(this);

    this->pushRenderStep(new LeftGutterTextRenderStep(this));
    this->pushRenderStep(new LineTextRenderStep(this));
    this->pushRenderStep(new CaretRenderStep(this));
    this->pushRenderStep(new CompilerIssueRenderStep(this));
    this->pushRenderStep(new CompilerIssueBackgroundRenderStep(this));
    this->pushRenderStep(new ActiveLineBackgroundRenderStep(this));

    this->repositionElements();
    this->setLineProperty(3, CompilationError, "Unknown type name 'Line'");
    this->setLineProperty(1, CompilationWarning, "Unused type 'FMP'");
}

TextEditor::~TextEditor() {
    delete d;
}

void TextEditor::undo() {
    d->undoStack->undo();
}

void TextEditor::redo() {
    d->undoStack->redo();
}

void TextEditor::setLineProperty(int line, KnownLineProperty property, QVariant value) {
    if (TextEditorPrivate::multiLineProperties.contains(property)) {
        this->setLinePropertyMulti(line, TextEditorPrivate::lineProperties.value(property), value);
    } else {
        this->setLineProperty(line, TextEditorPrivate::lineProperties.value(property), value);
    }
    emit knownLinePropertyChanged(line, property);
}

void TextEditor::setLineProperty(int line, QString property, QVariant value) {
    d->lines.at(line)->properties.remove(property);
    d->lines.at(line)->properties.insert(property, value);
    emit linePropertyChanged(line, property);
}

void TextEditor::setLinePropertyMulti(int line, QString property, QVariant value) {
    d->lines.at(line)->properties.insert(property, value);
    emit linePropertyChanged(line, property);
}

void TextEditor::clearLineProperties(QString property) {
    for (auto* line : d->lines) {
        line->properties.remove(property);
    }
}

QVariantList TextEditor::lineProperties(int line, KnownLineProperty property) {
    return this->lineProperties(line, TextEditorPrivate::lineProperties.value(property));
}

QVariantList TextEditor::lineProperties(int line, QString property) {
    return d->lines.at(line)->properties.values(property);
}

void TextEditor::pushRenderStep(TextEditorRenderStep* renderStep) {
    d->additionalRenderSteps.append(renderStep);
    std::sort(d->additionalRenderSteps.begin(), d->additionalRenderSteps.end(), [=](TextEditorRenderStep* first, TextEditorRenderStep* second) {
        return first->priority() < second->priority();
    });
}

TextEditorColorScheme* TextEditor::colorScheme() {
    return d->colorScheme;
}

QScrollBar* TextEditor::verticalScrollBar() {
    return d->vScrollBar;
}

QScrollBar* TextEditor::horizontalScrollBar() {
    return d->hScrollBar;
}

int TextEditor::numLines() {
    return d->lines.length();
}

int TextEditor::lineAtY(int y) {
    for (int i = 0; i < d->lines.length(); i++) {
        int top = lineTop(i);
        int bottom = lineHeight(i) + top;
        if (top < y && bottom >= y) return i;
    }
    return -1;
}

void TextEditor::repositionElements() {
    QRect vsGeometry;
    vsGeometry.setWidth(d->vScrollBar->width());
    vsGeometry.setHeight(this->height() - this->style()->pixelMetric(QStyle::PM_ScrollBarExtent));
    vsGeometry.moveTop(0);
    vsGeometry.moveRight(this->width());
    d->vScrollBar->setGeometry(vsGeometry);
    d->vScrollBar->setPageStep(this->height());

    QRect hsGeometry;
    hsGeometry.setHeight(d->hScrollBar->height());
    hsGeometry.setWidth(this->width() - this->style()->pixelMetric(QStyle::PM_ScrollBarExtent));
    hsGeometry.moveLeft(0);
    hsGeometry.moveBottom(this->height());
    d->hScrollBar->setGeometry(hsGeometry);
    d->hScrollBar->setPageStep(this->width());

    int fullHeight = this->lineTop(d->lines.length() - 1) + this->lineHeight(d->lines.length() - 1) + this->height() / 2;
    d->vScrollBar->setMaximum(qMax(0, fullHeight - this->height()));
}

int TextEditor::lineTop(int line) {
    if (d->lineTops.length() > line) return d->lineTops.value(line);

    int top;
    if (line == 0) {
        top = 0;
    } else {
        top = lineTop(line - 1) + lineHeight(line);
    }
    d->lineTops.append(top);
    return top;
}

int TextEditor::lineHeight(int line) {
    return this->fontMetrics().height() + SC_DPI_W(4, this);
}

int TextEditor::firstLineOnScreen() {
    int line = lineAtY(d->vScrollBar->value());
    return line == -1 ? 0 : line;
}

int TextEditor::lastLineOnScreen() {
    int line = lineAtY(d->vScrollBar->value() + this->height());
    return line == -1 ? d->lines.length() - 1 : line;
}

QString TextEditor::text() {
    QString text;
    for (auto line = d->lines.cbegin(); line != d->lines.cend(); line++) {
        // TODO: Configurable line endings
        text.append((*line)->contents);
        if (line != d->lines.cend() - 1) text.append("\n");
    }
    return text;
}

void TextEditor::setText(QString text) {
    d->undoStack->clear();

    // Clear all the carets
    for (auto caret : d->carets) {
        if (caret->isPrimary()) {
            caret->moveCaret(0, 0);
        } else {
            caret->discontinueAndDelete();
        }
    }

    d->lineProperties.clear();
    d->multiLineProperties.clear();

    qDeleteAll(d->lines);
    d->lines.clear();

    for (const auto& line : text.split("\n")) {
        auto* l = new TextEditorPrivate::Line;
        l->contents = line;
        d->lines.append(l);
    }

    this->repositionElements();
}

void TextEditor::setCurrentFile(QUrl currentFile) {
    d->currentFile = currentFile;
    emit currentFileChanged(currentFile);
}

QUrl TextEditor::currentFile() {
    return d->currentFile;
}

bool TextEditor::haveUnsavedChanges() {
    return !d->undoStack->isClean();
}

void TextEditor::setChangesSaved() {
    d->undoStack->setClean();
}

QRect TextEditor::renderStepOutputArea(QString stepName) {
    return renderStepOutputAreas().value(stepName);
}

QMenu* TextEditor::standardContextMenu() {
    auto* menu = new QMenu();
    menu->addAction(QIcon::fromTheme("edit-copy"), tr("Copy"), this, &TextEditor::copy);
    menu->addAction(QIcon::fromTheme("edit-cut"), tr("Cut"), this, &TextEditor::cut);
    menu->addAction(QIcon::fromTheme("edit-paste"), tr("Paste"), this, &TextEditor::paste);

    return menu;
}

void TextEditor::setLineEndingType(LineEndingType lineEndingType) {
    d->lineEndingType = lineEndingType;
    emit lineEndingTypeChanged();
}

TextEditor::LineEndingType TextEditor::currentLineEndingType() {
    return d->lineEndingType;
}

QString TextEditor::lineEndingString() {
    switch (d->lineEndingType) {
        case TextEditor::UnixLineEndings:
            return "\n";
        case TextEditor::WinLineEndings:
            return "\r\n";
        case TextEditor::ClassicMacLineEndings:
            return "\r";
    }
    return "\n";
}

QString TextEditor::selectedText() {
    return selectedCarets().join("\n");
}

QStringList TextEditor::selectedCarets() {
    QStringList selected;
    for (auto* caret : d->carets) {
        selected.append(caret->textBetweenAnchors());
    }
    return selected;
}

void TextEditor::copy() {
    if (this->selectedText().isEmpty()) return;

    QApplication::clipboard()->setText(this->selectedText());
}

void TextEditor::cut() {
    if (this->selectedText().isEmpty()) return;

    QApplication::clipboard()->setText(this->selectedText());
    d->undoStack->push(new CaretEraseCommand(this, true));
}

void TextEditor::paste() {
    d->undoStack->push(new CaretTextCommand(this, QApplication::clipboard()->text()));
}

QRect TextEditor::characterRect(QPoint linePos) {
    QString contents = d->lines.at(linePos.y())->contents;

    int xOffset = renderStepOutputArea("LineText").left() - d->hScrollBar->value();

    QRect r;
    r.setTop(lineTop(linePos.y()) - d->vScrollBar->value());
    r.setHeight(lineHeight(linePos.y()));
    r.setLeft(this->fontMetrics().horizontalAdvance(contents.left(linePos.x())) + xOffset);
    if (linePos.x() + 1 == contents.length() + 1) {
        r.setRight(this->width());
    } else {
        r.setRight(this->fontMetrics().horizontalAdvance(contents.left(linePos.x() + 1)) + xOffset);
    }
    return r;
}

void TextEditor::drawLine(int line, QPainter* painter) {
    QVariantList compilationErrors = this->lineProperties(line, CompilationError);
    QVariantList compilationWarnings = this->lineProperties(line, CompilationWarning);

    QRect lineRect;
    lineRect.setHeight(lineHeight(line));
    lineRect.setWidth(this->width());
    lineRect.moveTop(lineTop(line) - d->vScrollBar->value());
    lineRect.moveLeft(0 - d->hScrollBar->value());

    bool isActiveLine = false;
    for (TextCaret* caret : d->carets) {
        if (caret->linePos().y() == line) isActiveLine = true;

        // Don't draw any active line highlights if there is an active selection
        if (caret->firstAnchor() != caret->lastAnchor()) {
            isActiveLine = false;
            break;
        }
    }

    painter->save();

    if (!compilationErrors.isEmpty()) {
        painter->fillRect(lineRect, this->colorScheme()->item(TextEditorColorScheme::LineWithCompilationError).color());
    } else if (!compilationWarnings.isEmpty()) {
        painter->fillRect(lineRect, this->colorScheme()->item(TextEditorColorScheme::LineWithCompilationWarning).color());
    }

    // Draw active line highlight
    if (isActiveLine) {
        painter->fillRect(lineRect, this->colorScheme()->item(TextEditorColorScheme::ActiveLine).color());
        painter->setPen(this->colorScheme()->item(TextEditorColorScheme::ActiveLineMarginText).color());
    } else {
        painter->setPen(this->colorScheme()->item(TextEditorColorScheme::MarginText).color());
    }

    painter->restore();
}

void TextEditor::addCaret(int line, int pos) {
    TextCaret* caret = new TextCaret(line, pos, this);
    connect(caret, &TextCaret::discontinued, this, [=] {
        d->carets.removeAll(caret);
    });
    d->carets.append(caret);

    this->update();
}

void TextEditor::addCaret(QPoint linePos) {
    addCaret(linePos.y(), linePos.x());
}

void TextEditor::simplifyCarets() {
    if (d->simplifyingCarets) return;
    d->simplifyingCarets = true;

    for (TextCaret* caret : d->carets) {
        if (caret->isDiscontinued()) continue;
        bool hasSelection = caret->firstAnchor() != caret->lastAnchor();
        for (TextCaret* caret2 : d->carets) {
            if (caret2 == caret) continue;
            if (caret2->isDiscontinued()) continue;
            if (caret2->isPrimary()) continue;

            bool hasSelection2 = caret2->firstAnchor() != caret2->lastAnchor();
            if (hasSelection && hasSelection2) {
                // Merge the selection if required
                if (this->linePosToChar(caret2->firstAnchor()) > this->linePosToChar(caret->lastAnchor()) || this->linePosToChar(caret2->lastAnchor()) < this->linePosToChar(caret->firstAnchor())) {
                    // These two selections don't intersect
                } else {
                    // Merge the selections
                    QPoint firstAnchor = this->charToLinePos(qMin(this->linePosToChar(caret->firstAnchor()), this->linePosToChar(caret2->firstAnchor())));
                    QPoint lastAnchor = this->charToLinePos(qMax(this->linePosToChar(caret->lastAnchor()), this->linePosToChar(caret2->lastAnchor())));

                    caret->moveCaret(firstAnchor);
                    caret->setAnchor(lastAnchor);
                    caret2->discontinueAndDelete();
                }
            } else if (hasSelection) {
                // If caret2 is in between the selection, discontinue it
                int linePosChar = this->linePosToChar(caret2->linePos());
                if (this->linePosToChar(caret->firstAnchor()) < linePosChar && this->linePosToChar(caret->lastAnchor()) > linePosChar) caret2->discontinueAndDelete();
            } else {
                if (caret->linePos() == caret2->linePos()) caret2->discontinueAndDelete();
            }
        }
    }

    d->simplifyingCarets = false;
}

QPoint TextEditor::hitTest(QPoint pos) {
    int translatedMouseX = pos.x() - renderStepOutputArea("LineText").left() + d->hScrollBar->value();
    int translatedMouseY = pos.y() + d->vScrollBar->value();

    int selectedLine = this->lastLineOnScreen();
    for (int i = firstLineOnScreen(); i <= this->lastLineOnScreen(); i++) {
        int top = lineTop(i);
        int bottom = top + lineHeight(i);
        if (translatedMouseY >= top && translatedMouseY < bottom) {
            selectedLine = i;
            break;
        }
    }

    QString lineContents = d->lines.at(selectedLine)->contents;
    int lastX = -30000;
    for (int i = 0; i < lineContents.length(); i++) {
        int x = this->fontMetrics().horizontalAdvance(lineContents.left(i));
        if (x > translatedMouseX) {
            if (translatedMouseX - lastX > x - translatedMouseX) {
                return QPoint(i, selectedLine);
            } else {
                return QPoint(i - 1, selectedLine);
            }
        }
        lastX = x;
    }

    return QPoint(lineContents.length(), selectedLine);
}

int TextEditor::linePosToChar(QPoint linePos) {
    int c = linePos.x();
    for (int i = 0; i < linePos.y(); i++) {
        c += d->lines.at(i)->contents.length() + 1;
    }
    return c;
}

QPoint TextEditor::charToLinePos(int c) {
    if (c < 0) c = 0;

    int charsLeft = c;
    for (int i = 0; i < d->lines.length(); i++) {
        c -= d->lines.at(i)->contents.length() + 1;
        if (c < 0) {
            return QPoint(d->lines.at(i)->contents.length() + c + 1, i);
        }
    }
    return QPoint(d->lines.last()->contents.length() + 1, d->lines.length() - 1);
}

QMap<QString, QRect> TextEditor::renderStepOutputAreas() {
    QRect baseRect;
    baseRect.setTop(0);
    baseRect.setHeight(this->height());

    QMap<QString, QRect> stepRenderRects;
    int currentLeftMargin = 0;
    int currentRightMargin = this->width();
    for (TextEditorRenderStep* step : d->additionalRenderSteps) {
        if (step->renderSide() == TextEditorRenderStep::Left) {
            QRect r = baseRect;
            r.setWidth(step->renderWidth());
            r.moveLeft(currentLeftMargin);
            stepRenderRects.insert(step->stepName(), r);
            currentLeftMargin = r.right();
        } else if (step->renderSide() == TextEditorRenderStep::Right) {
            QRect r = baseRect;
            r.setWidth(step->renderWidth());
            r.moveRight(currentRightMargin);
            stepRenderRects.insert(step->stepName(), r);
            currentRightMargin = r.left();
        }
    }

    for (TextEditorRenderStep* step : d->additionalRenderSteps) {
        if (!stepRenderRects.contains(step->stepName())) {
            if (step->renderSide() == TextEditorRenderStep::Center) {
                QRect renderRect = baseRect;
                renderRect.setLeft(currentLeftMargin);
                renderRect.setRight(currentRightMargin);
                stepRenderRects.insert(step->stepName(), renderRect);
            } else {
                stepRenderRects.insert(step->stepName(), stepRenderRects.value(step->renderStack()));
            }
        }
    }
    return stepRenderRects;
}

void TextEditor::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.fillRect(QRect(0, 0, this->width(), this->height()), this->colorScheme()->item(TextEditorColorScheme::Background));

    QMap<QString, QRect> stepRenderRects = renderStepOutputAreas();
    for (TextEditorRenderStep* step : d->additionalRenderSteps) {
        step->paint(&painter, stepRenderRects.value(step->stepName()), event->rect());
    }
}

void TextEditor::resizeEvent(QResizeEvent* event) {
    this->repositionElements();
}

void TextEditor::wheelEvent(QWheelEvent* event) {
    d->vScrollBar->setValue(d->vScrollBar->value() - event->pixelDelta().y());
    d->hScrollBar->setValue(d->hScrollBar->value() - event->pixelDelta().x());
}

void TextEditor::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        event->accept();

        // Set the caret
        if (event->modifiers() == (Qt::ControlModifier | Qt::AltModifier)) {
            addCaret(hitTest(event->pos()));
        } else {
            for (TextCaret* caret : d->carets) {
                if (caret->isPrimary()) {
                    if (event->modifiers() == Qt::ShiftModifier) {
                        caret->setAnchor(hitTest(event->pos()));
                    } else {
                        caret->moveCaret(hitTest(event->pos()));
                    }
                    d->draggingCaret = caret;
                } else {
                    caret->discontinueAndDelete();
                }
            }
        }
    }
}

void TextEditor::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        event->accept();
        d->draggingCaret = nullptr;
    }
}

void TextEditor::mouseMoveEvent(QMouseEvent* event) {
    for (auto step = d->additionalRenderSteps.rbegin(); step != d->additionalRenderSteps.rend(); step++) {
        if ((*step)->mouseMoveEvent(event)) return;
    }

    //    if (event->pos().x() + d->hScrollBar->value() < leftMarginWidth()) {
    //        this->setCursor(QCursor(Qt::ArrowCursor));
    //    } else {
    this->setCursor(QCursor(Qt::IBeamCursor));
    //    }

    if (event->buttons() & Qt::LeftButton) {
        if (d->draggingCaret) {
            d->draggingCaret->setAnchor(hitTest(event->pos()));
            this->update();
            return;
        }
    }
}

void TextEditor::keyPressEvent(QKeyEvent* event) {
    Qt::KeyboardModifiers modifiers = event->modifiers() & ~Qt::KeypadModifier;

    if (modifiers == Qt::NoModifier || modifiers == Qt::ShiftModifier) {
        if (event->key() == Qt::Key_Backspace) {
            d->undoStack->push(new CaretEraseCommand(this, true));
        } else if (event->key() == Qt::Key_Delete) {
            d->undoStack->push(new CaretEraseCommand(this, false));
        } else if (event->key() == Qt::Key_Return) {
            d->undoStack->push(new CaretTextCommand(this, QStringLiteral("\n")));
        } else if (!event->text().isEmpty()) {
            d->undoStack->push(new CaretTextCommand(this, event->text()));
        }
    }

    if (modifiers == Qt::NoModifier) {
        if (event->key() == Qt::Key_Up) {
            for (TextCaret* caret : d->carets) {
                caret->moveCaretRelative(-1, 0);
            }
            this->simplifyCarets();
        } else if (event->key() == Qt::Key_Down) {
            for (TextCaret* caret : d->carets) {
                caret->moveCaretRelative(1, 0);
            }
            this->simplifyCarets();
        } else if (event->key() == Qt::Key_Left) {
            for (TextCaret* caret : d->carets) {
                caret->moveCaretRelative(0, -1);
            }
            this->simplifyCarets();
        } else if (event->key() == Qt::Key_Right) {
            for (TextCaret* caret : d->carets) {
                caret->moveCaretRelative(0, 1);
            }
            this->simplifyCarets();
        } else if (event->key() == Qt::Key_End) {
            for (TextCaret* caret : d->carets) {
                caret->moveCaretToEndOfLine();
            }
            this->simplifyCarets();
        } else if (event->key() == Qt::Key_Home) {
            for (TextCaret* caret : d->carets) {
                caret->moveCaretToStartOfLine();
            }
            this->simplifyCarets();
        }

        this->update();
    } else if (modifiers == Qt::ShiftModifier) {
        if (event->key() == Qt::Key_Up) {
            for (TextCaret* caret : d->carets) {
                caret->moveAnchorRelative(-1, 0);
            }
            this->simplifyCarets();
        } else if (event->key() == Qt::Key_Down) {
            for (TextCaret* caret : d->carets) {
                caret->moveAnchorRelative(1, 0);
            }
            this->simplifyCarets();
        } else if (event->key() == Qt::Key_Left) {
            for (TextCaret* caret : d->carets) {
                caret->moveAnchorRelative(0, -1);
            }
            this->simplifyCarets();
        } else if (event->key() == Qt::Key_Right) {
            for (TextCaret* caret : d->carets) {
                caret->moveAnchorRelative(0, 1);
            }
            this->simplifyCarets();
        } else if (event->key() == Qt::Key_End) {
            for (TextCaret* caret : d->carets) {
                //                caret->moveCaretToEndOfLine();
            }
            this->simplifyCarets();
        } else if (event->key() == Qt::Key_Home) {
            for (TextCaret* caret : d->carets) {
                //                caret->moveCaretToStartOfLine();
            }
            this->simplifyCarets();
        }

        this->update();
    }

    //        d->editor->update();
    //    return false;
}

void TextEditor::keyReleaseEvent(QKeyEvent* event) {
}

SavedCarets TextEditorPrivate::saveCarets() {
    QList<TextCaret::SavedCaret> carets;
    for (TextCaret* caret : this->carets) {
        carets.append(caret->saveCaret());
    }
    return carets;
}

void TextEditorPrivate::loadCarets(SavedCarets carets) {
    if (carets.isEmpty()) return;

    QList<TextCaret*> cs;
    for (TextCaret::SavedCaret c : carets) {
        if (this->carets.isEmpty()) {
            TextCaret* editingCaret = TextCaret::fromSavedCaret(c);
            cs.append(editingCaret);
        } else {
            TextCaret* editingCaret = this->carets.takeFirst();
            editingCaret->loadCaret(c);
            cs.append(editingCaret);
        }
    }

    for (TextCaret* oldCaret : this->carets) {
        oldCaret->discontinueAndDelete();
    }

    this->carets = cs;
}

void TextEditor::contextMenuEvent(QContextMenuEvent* event) {
    QMenu* menu = this->standardContextMenu();
    connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);
    menu->popup(event->globalPos());
}
