#ifndef TEXTEDITOR_P_H
#define TEXTEDITOR_P_H

#include "textcaret.h"
#include "texteditor.h"
#include <QMultiMap>
#include <QScrollBar>
#include <QString>
#include <QUndoStack>

struct TextEditorPrivate {
        struct Line {
                QString contents;
                QMultiMap<QString, QVariant> properties;
        };

        QList<Line*> lines;
        QScrollBar *vScrollBar, *hScrollBar;
        QList<int> lineTops;

        QList<TextCaret*> carets;
        TextCaret* draggingCaret = nullptr;
        QUndoStack* undoStack;

        SavedCarets saveCarets();
        void loadCarets(SavedCarets carets);

        TextEditorColorScheme* colorScheme;

        static QMap<TextEditor::KnownLineProperty, QString> lineProperties;
        static QList<TextEditor::KnownLineProperty> multiLineProperties;
};

#endif // TEXTEDITOR_P_H
