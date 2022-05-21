#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <QWidget>

class QScrollBar;
class TextEditorColorScheme;
class TextCaret;
class CaretTextCommand;
class CaretEraseCommand;
class TextEditorCommand;
class TextEditorRenderStep;
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

        int lineAtY(int y);
        int lineTop(int line);
        int lineHeight(int line);
        int firstLineOnScreen();
        int lastLineOnScreen();

    signals:

    protected:
        friend TextCaret;
        friend CaretTextCommand;
        friend CaretEraseCommand;
        friend TextEditorCommand;
        TextEditorPrivate* d;

    private:
        void repositionElements();

        int leftMarginWidth();

        QRect characterRect(QPoint linePos);

        void drawLine(int line, QPainter* painter);
        void addCaret(int line, int pos);
        void addCaret(QPoint linePos);
        void simplifyCarets();

        QPoint hitTest(QPoint pos);

        int linePosToChar(QPoint linePos);
        QPoint charToLinePos(int c);

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
};

#endif // TEXTEDITOR_H
