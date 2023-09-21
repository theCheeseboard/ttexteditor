#include "texteditorsyntaxhighlighter.h"

#include "texteditor.h"
#include "texteditor_p.h"
#include <KSyntaxHighlighting/definition.h>
#include <KSyntaxHighlighting/repository.h>
#include <KSyntaxHighlighting/state.h>
#include <ranges/trange.h>

struct TextEditorSyntaxHighlighterPrivate {
        KSyntaxHighlighting::Repository repo;
        TextEditor* parent;

        int line = 0;
};

TextEditorSyntaxHighlighter::TextEditorSyntaxHighlighter() :
    KSyntaxHighlighting::AbstractHighlighter{} {
    Q_INIT_RESOURCE(ttexteditor_themes);

    d = new TextEditorSyntaxHighlighterPrivate();
    d->repo.addCustomSearchPath(":/ttexteditor/syntaxhighlightingresources");
    this->setTheme(d->repo.theme("Contemporary Dark"));
    //    this->setDefinition(d->repo.definitionForMimeType("application/json"));
    this->setDefinition(d->repo.definitionForName("C++"));
    //    this->setTheme(d->repo.defaultTheme(KSyntaxHighlighting::Repository::DarkTheme));
}

TextEditorSyntaxHighlighter::~TextEditorSyntaxHighlighter() {
    delete d;
}

void TextEditorSyntaxHighlighter::setTextEditor(TextEditor* textEditor) {
    d->parent = textEditor;
}

void TextEditorSyntaxHighlighter::highlightFrom(int line) {
    KSyntaxHighlighting::State state;
    while (line != 0) {
        // TODO: Retrieve the state
        auto states = d->parent->lineProperties(line - 1, TextEditor::HighlightState);
        if (states.isEmpty()) {
            line--;
        } else {
            state = states.constFirst().value<KSyntaxHighlighting::State>();
            break;
        }
    }

    for (auto l = line; l <= d->parent->lastLineOnScreen(); l++) {
        d->line = l;
        auto textLine = d->parent->d->lines.at(l);

        d->parent->clearLineProperty(l, TextEditor::HighlightFormat);
        state = this->highlightLine(textLine->contents, state);

        d->parent->setLineProperty(l, TextEditor::HighlightState, QVariant::fromValue(state));
    }

    for (auto l = d->parent->lastLineOnScreen(); l < d->parent->numLines(); l++) {
        d->parent->clearLineProperty(l, TextEditor::HighlightState);
    }
}

KSyntaxHighlighting::Format TextEditorSyntaxHighlighter::formatForId(int id) {
    return tRange(this->definition().formats()).first([id](const KSyntaxHighlighting::Format& format) {
        return format.id() == id;
    });
}

void TextEditorSyntaxHighlighter::applyFormat(int offset, int length, const KSyntaxHighlighting::Format& format) {
    d->parent->setLineProperty(d->line, TextEditor::HighlightFormat, QVariant::fromValue(TextEditorSyntaxHighlighterFormat{offset, length, format}));
}
