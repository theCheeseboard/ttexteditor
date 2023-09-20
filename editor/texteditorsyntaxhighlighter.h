#ifndef TEXTEDITORSYNTAXHIGHLIGHTER_H
#define TEXTEDITORSYNTAXHIGHLIGHTER_H

#include <KSyntaxHighlighting/abstracthighlighter.h>
#include <KSyntaxHighlighting/format.h>

class TextEditor;
struct TextEditorSyntaxHighlighterPrivate;
class TextEditorSyntaxHighlighter : public KSyntaxHighlighting::AbstractHighlighter {
    public:
        explicit TextEditorSyntaxHighlighter();
        ~TextEditorSyntaxHighlighter();

        void setTextEditor(TextEditor* textEditor);
        void highlightFrom(int line);

        KSyntaxHighlighting::Format formatForId(int id);

    private:
        TextEditorSyntaxHighlighterPrivate* d;

        // AbstractHighlighter interface
    protected:
        void applyFormat(int offset, int length, const KSyntaxHighlighting::Format& format);
};

struct TextEditorSyntaxHighlighterFormat {
        int offset;
        int length;
        KSyntaxHighlighting::Format format;
};

#endif // TEXTEDITORSYNTAXHIGHLIGHTER_H
