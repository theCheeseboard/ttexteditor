#ifndef TEXTREPLACEMENTCOMMAND_H
#define TEXTREPLACEMENTCOMMAND_H

#include <QUndoCommand>

class TextEditor;
struct TextReplacementCommandPrivate;
class TextReplacementCommand : public QUndoCommand {
    public:
        TextReplacementCommand(TextEditor* editor, QPoint anchorStart, QPoint anchorEnd, QString replacement);
        ~TextReplacementCommand();

    private:
        TextReplacementCommandPrivate* d;

        void replace(QPoint anchorStart, QPoint anchorEnd, QString replacement);

        // QUndoCommand interface
    public:
        void undo();
        void redo();
        int id() const;
};

#endif // TEXTREPLACEMENTCOMMAND_H
