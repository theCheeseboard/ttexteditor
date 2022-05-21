#include "compilerissuerenderstep.h"

#include "../texteditor.h"
#include <QMouseEvent>
#include <QPainter>
#include <QScrollBar>
#include <tvariantanimation.h>

struct CompilerIssueRenderStepPrivate {
        tVariantAnimation* expandingAnimation;
        int expandedWidth = 0;
        bool expanded = false;
};

CompilerIssueRenderStep::CompilerIssueRenderStep(TextEditor* parent) :
    TextEditorRenderStep{parent} {
    d = new CompilerIssueRenderStepPrivate();

    d->expandingAnimation = new tVariantAnimation();
    d->expandingAnimation->setStartValue(0.0);
    d->expandingAnimation->setEndValue(0.0);
    d->expandingAnimation->setDuration(250);
    d->expandingAnimation->setEasingCurve(QEasingCurve::OutCubic);
    connect(d->expandingAnimation, &tVariantAnimation::valueChanged, parent, QOverload<>::of(&TextEditor::update));

    connect(parent, &TextEditor::knownLinePropertyChanged, this, [=](int line, TextEditor::KnownLineProperty property) {
        if (property == TextEditor::CompilationError || property == TextEditor::CompilationWarning) updateExpandedWidth();
    });
    updateExpandedWidth();
}

CompilerIssueRenderStep::~CompilerIssueRenderStep() {
    delete d;
}

void CompilerIssueRenderStep::updateExpandedWidth() {
    TextEditor* parent = this->parentEditor();
    QFontMetrics metrics = parent->fontMetrics();

    int longestWidth = 0;
    for (int i = 0; i < parent->numLines(); i++) {
        QVariantList compilationErrors = parent->lineProperties(i, TextEditor::CompilationError);
        QVariantList compilationWarnings = parent->lineProperties(i, TextEditor::CompilationWarning);

        for (const auto& error : compilationErrors) {
            longestWidth = qMax(longestWidth, metrics.horizontalAdvance(error.toString()));
        }
        for (const auto& warning : compilationWarnings) {
            longestWidth = qMax(longestWidth, metrics.horizontalAdvance(warning.toString()));
        }
    }
    d->expandedWidth = longestWidth + SC_DPI_W(30, parent);
}

int CompilerIssueRenderStep::errorColWidth() {
    TextEditor* parent = this->parentEditor();
    int lowWidth = SC_DPI_W(24, parent) + parent->verticalScrollBar()->width();
    int highWidth = d->expandedWidth;
    int range = highWidth - lowWidth;
    int selectedRange = range * d->expandingAnimation->currentValue().toReal();
    return lowWidth + selectedRange;
}

void CompilerIssueRenderStep::paint(QPainter* painter, QRect redrawBounds) {
    TextEditor* parent = this->parentEditor();

    painter->save();

    int firstLine = parent->lineAtY(redrawBounds.top());
    if (firstLine == -1) firstLine = parent->firstLineOnScreen();
    int lastLine = parent->lineAtY(redrawBounds.bottom());
    if (lastLine == -1) lastLine = parent->lastLineOnScreen();
    for (int i = firstLine; i <= lastLine; i++) {
        QVariantList compilationErrors = parent->lineProperties(i, TextEditor::CompilationError);
        QVariantList compilationWarnings = parent->lineProperties(i, TextEditor::CompilationWarning);

        auto drawLineAnnotation = [=](QColor background, QString str) {
            QRect lineRect;
            lineRect.setHeight(parent->lineHeight(i));
            lineRect.moveTop(parent->lineTop(i) - parent->verticalScrollBar()->value());
            lineRect.setWidth(this->errorColWidth());
            lineRect.moveRight(parent->width());
            painter->fillRect(lineRect, background);

            painter->setClipRect(lineRect);
            painter->setClipping(true);

            QRect textRect = lineRect;
            textRect.moveLeft(textRect.left() + SC_DPI_W(2, parent));
            textRect.setWidth(painter->fontMetrics().horizontalAdvance(str) + SC_DPI_W(2, parent));
            painter->setPen(Qt::white);
            painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, str);

            painter->setClipping(false);
        };

        if (!compilationErrors.isEmpty()) {
            drawLineAnnotation(Qt::red, compilationErrors.first().toString());
        } else if (!compilationWarnings.isEmpty()) {
            drawLineAnnotation(QColor(200, 100, 0), compilationWarnings.first().toString());
        }
    }

    painter->restore();
}

bool CompilerIssueRenderStep::mouseMoveEvent(QMouseEvent* event) {
    int x = event->pos().x();
    int triggerX = parentEditor()->width() - errorColWidth();
    if (x > triggerX && !d->expanded) {
        // Expand error pane
        d->expandingAnimation->stop();
        d->expandingAnimation->setStartValue(d->expandingAnimation->currentValue());
        d->expandingAnimation->setEndValue(1.0);
        d->expandingAnimation->start();
        d->expanded = true;
    } else if (x < triggerX && d->expanded) {
        // Collapse error pane
        d->expandingAnimation->stop();
        d->expandingAnimation->setStartValue(d->expandingAnimation->currentValue());
        d->expandingAnimation->setEndValue(0.0);
        d->expandingAnimation->start();
        d->expanded = false;
    }
    return false;
}
