#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include "ttexteditor_global.h"
#include <QOpenGLWidget>
#include <QUrl>

struct TextDelta {
        QPoint startEdit;
        QPoint endEdit;
        QString replacement;
};

class QScrollBar;
class TextEditorColorScheme;
class TextCaret;
class CaretRenderStep;
class TextEditorCommand;
class TextEditorRenderStep;
class LineTextRenderStep;
class CaretTextCommand;
class CaretEraseCommand;
class CaretTabCommand;
class TextReplacementCommand;
class LeftGutterTextRenderStep;
class ActiveLineBackgroundRenderStep;
class TextEditorSyntaxHighlighter;
class QMenu;
struct TextEditorPrivate;
class TTEXTEDITOR_EXPORT TextEditor : public QWidget {
        Q_OBJECT
    public:
        explicit TextEditor(QWidget* parent = nullptr);
        ~TextEditor();

        void undo();
        void redo();

        bool readOnly();
        void setReadOnly(bool readOnly);

        int tabLength();
        void setTabLength(int tabLength);

        bool preferSpaces();
        void setPreferSpaces(bool preferSpaces);

        enum KnownLineProperty {
            CompilationWarning,
            CompilationError,
            HighlightState,
            HighlightFormat
        };

        void setLineProperty(int line, KnownLineProperty property, QVariant value);
        void setLineProperty(int line, QString property, QVariant value);
        void setLinePropertyMulti(int line, QString property, QVariant value);
        void clearLineProperties(QString property);
        void clearLineProperties(KnownLineProperty property);
        void clearLineProperty(int line, KnownLineProperty property);
        QVariantList lineProperties(int line, KnownLineProperty property);
        QVariantList lineProperties(int line, QString property);

        void pushRenderStep(TextEditorRenderStep* renderStep);

        TextEditorColorScheme* colorScheme();

        QScrollBar* verticalScrollBar();
        QScrollBar* horizontalScrollBar();

        int numLines();
        int lineAtY(int y);
        int lineTop(int line);
        int lineHeight(int line);
        int firstLineOnScreen();
        int lastLineOnScreen();

        QString text();
        void setText(QString text);
        void setCurrentFile(QUrl currentFile);
        QUrl currentFile();
        bool haveUnsavedChanges();
        void setChangesSaved();

        QRect renderStepOutputArea(QString stepName);

        QMenu* standardContextMenu();

        enum LineEndingType {
            UnixLineEndings,
            WinLineEndings,
            ClassicMacLineEndings
        };

        void setLineEndingType(LineEndingType lineEndingType);
        LineEndingType currentLineEndingType();
        QString lineEndingString();

        QString selectedText();
        QStringList selectedCarets();

        int numberOfCarets();
        QPoint caretAnchorStart(int caret);
        QPoint caretAnchorEnd(int caret);
        void scrollToPrimaryCaret();

        void replaceText(QPoint anchorStart, QPoint anchorEnd, QString replacement);

        void copy();
        void cut();
        void paste();

        QPoint hitTest(QPoint pos);
        QRect characterRect(QPoint linePos, bool withScrollBars = true);

    signals:
        void knownLinePropertyChanged(int line, TextEditor::KnownLineProperty property);
        void linePropertyChanged(int line, QString property);
        void currentFileChanged(QUrl currentFile);
        void unsavedChangesChanged();
        void lineEndingTypeChanged();
        void textChanged(QList<TextDelta> deltas);
        void readOnlyChanged(bool readOnly);
        void keyTyped(QString keyText);

    protected:
        friend TextCaret;
        friend CaretRenderStep;
        friend TextEditorCommand;
        friend CaretTextCommand;
        friend CaretEraseCommand;
        friend CaretTabCommand;
        friend LineTextRenderStep;
        friend LeftGutterTextRenderStep;
        friend ActiveLineBackgroundRenderStep;
        friend TextReplacementCommand;
        friend TextEditorSyntaxHighlighter;
        TextEditorPrivate* d;

        void signalTextChanged(QList<TextDelta> deltas);

    private:
        void repositionElements();

        void drawLine(int line, QPainter* painter);
        void addCaret(int line, int pos);
        void addCaret(QPoint linePos);
        void simplifyCarets();

        int linePosToChar(QPoint linePos);
        QPoint charToLinePos(int c);

        QMap<QString, QRect> renderStepOutputAreas();

        // QWidget interface
    protected:
        void paintEvent(QPaintEvent* event);
        void resizeEvent(QResizeEvent* event);
        void wheelEvent(QWheelEvent* event);

        // QWidget interface
    protected:
        void mousePressEvent(QMouseEvent* event);
        void mouseReleaseEvent(QMouseEvent* event);
        void mouseMoveEvent(QMouseEvent* event);

        // QWidget interface
    protected:
        void keyPressEvent(QKeyEvent* event);
        void keyReleaseEvent(QKeyEvent* event);

        // QWidget interface
    protected:
        void contextMenuEvent(QContextMenuEvent* event);

        // QWidget interface
    protected:
        bool focusNextPrevChild(bool next);
};

#endif // TEXTEDITOR_H
