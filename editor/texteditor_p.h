#ifndef TEXTEDITOR_P_H
#define TEXTEDITOR_P_H

#include "textcaret.h"
#include "texteditor.h"
#include <QMultiMap>
#include <QScrollBar>
#include <QString>
#include <QUndoStack>
#include <QUrl>

class TextEditorRenderStep;
struct TextEditorPrivate {
        struct Line {
                QString contents;
                QMultiMap<QString, QVariant> properties;
        };

        QList<Line*> lines;
        QScrollBar *vScrollBar, *hScrollBar;
        QList<int> lineTops;

        bool readOnly = false;
        int tabLength = 4;
        bool preferSpaces = true;

        QList<TextCaret*> carets;
        TextCaret* draggingCaret = nullptr;
        QUndoStack* undoStack;

        SavedCarets saveCarets();
        void loadCarets(SavedCarets carets);

        TextEditorColorScheme* colorScheme;

        bool simplifyingCarets = false;

        QList<TextEditorRenderStep*> additionalRenderSteps;

        QUrl currentFile;

#ifdef Q_OS_WIN
        TextEditor::LineEndingType lineEndingType = TextEditor::WinLineEndings;
#else
        TextEditor::LineEndingType lineEndingType = TextEditor::UnixLineEndings;
#endif

        static const QMap<TextEditor::KnownLineProperty, QString> lineProperties;
        static const QList<TextEditor::KnownLineProperty> multiLineProperties;
};

#endif // TEXTEDITOR_P_H
