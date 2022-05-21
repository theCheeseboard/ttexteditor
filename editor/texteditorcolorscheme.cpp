#include "texteditorcolorscheme.h"

#include <QBrush>

TextEditorColorScheme::TextEditorColorScheme(QObject* parent) :
    QObject{parent} {
}

QBrush TextEditorColorScheme::item(ColorSchemeItem item) {
    switch (item) {
        case TextEditorColorScheme::Background:
            return QColor(40, 40, 40);
        case TextEditorColorScheme::Margin:
            return QColor(40, 40, 40);
        case TextEditorColorScheme::NormalText:
            return QColor(255, 255, 255);
        case TextEditorColorScheme::MarginText:
            return QColor(200, 200, 200);
        case TextEditorColorScheme::Breakpoint:
            return QColor(50, 170, 80);
        case TextEditorColorScheme::ActiveLine:
            return QColor(100, 100, 100, 100);
        case TextEditorColorScheme::ActiveLineMarginText:
            return QColor(255, 255, 255);
        case TextEditorColorScheme::HighlightedText:
            return QColor(255, 255, 255, 100);
        case TextEditorColorScheme::HighlightedTextBorder:
            return QColor(255, 255, 255);
        case TextEditorColorScheme::LineWithCompilationError:
            return QColor(100, 40, 40);
        case TextEditorColorScheme::LineWithCompilationWarning:
            return QColor(100, 100, 40);
    }

    return QColor();
}
