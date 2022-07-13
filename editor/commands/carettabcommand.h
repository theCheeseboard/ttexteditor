#ifndef CARETTABCOMMAND_H
#define CARETTABCOMMAND_H

#include "texteditorcommand.h"

class CaretTabCommand : public TextEditorCommand {
    public:
        explicit CaretTabCommand(TextEditor* editor);
        ~CaretTabCommand();

        // QUndoCommand interface
    public:
        int id() const;
};

#endif // CARETTABCOMMAND_H
