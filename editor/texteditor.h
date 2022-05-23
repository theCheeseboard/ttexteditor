#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <QUrl>
#include <QWidget>

class QScrollBar;
class TextEditorColorScheme;
class TextCaret;
class CaretRenderStep;
class TextEditorCommand;
class TextEditorRenderStep;
class LineTextRenderStep;
class CaretTextCommand;
class CaretEraseCommand;
class LeftGutterTextRenderStep;
class ActiveLineBackgroundRenderStep;
class QMenu;
struct TextEditorPrivate;
class TextEditor : public QWidget {
        Q_OBJECT
    public:
        explicit TextEditor(QWidget* parent = nullptr);
        ~TextEditor();

        void undo();
        void redo();

        enum KnownLineProperty {
            CompilationWarning,
            CompilationError
        };

        void setLineProperty(int line, KnownLineProperty property, QVariant value);
        void setLineProperty(int line, QString property, QVariant value);
        void setLinePropertyMulti(int line, QString property, QVariant value);
        void clearLineProperties(QString property);
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

        QString lineEndingString();

        QString selectedText();
        QStringList selectedCarets();

        void copy();
        void cut();
        void paste();

    signals:
        void knownLinePropertyChanged(int line, TextEditor::KnownLineProperty property);
        void linePropertyChanged(int line, QString property);
        void currentFileChanged(QUrl currentFile);
        void unsavedChangesChanged();

    protected:
        friend TextCaret;
        friend CaretRenderStep;
        friend TextEditorCommand;
        friend CaretTextCommand;
        friend CaretEraseCommand;
        friend LineTextRenderStep;
        friend LeftGutterTextRenderStep;
        friend ActiveLineBackgroundRenderStep;
        TextEditorPrivate* d;

    private:
        void repositionElements();

        QRect characterRect(QPoint linePos);

        void drawLine(int line, QPainter* painter);
        void addCaret(int line, int pos);
        void addCaret(QPoint linePos);
        void simplifyCarets();

        QPoint hitTest(QPoint pos);

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
};

#endif // TEXTEDITOR_H
