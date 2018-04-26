
#ifndef KAIJU_PARSE_LEXER_H
#define KAIJU_PARSE_LEXER_H

#include "kaiju/IR/TranslationUnit.h"
#include "kaiju/Parse/Token.h"
#include "kaiju/Support/DiagnosticBuilder.h"
#include "kaiju/Support/Diagnostic.h"

namespace kaiju {

// Class Lexer
//
// \brief This is the Kaiju Lexer interface.
//
class Lexer {

    // \brief The TranslationUnit this lexer is working on.
    TranslationUnit &Unit;

    // \brief This is this Lexer's DiagnosticBuilder.
    DiagnosticBuilder &Builder;

    const char *Start,  //< \brief The beginning of the TranslationUnit's buffer
               *End;    //< \brief The end of the TranslationUnit's buffer.

    // \brief The point within this lexer's buffer than we're reading from.
    const char *Pointer;

    // \brief Returns a pointer to the spot this lexer is lexing from in
    // the buffer.
    inline const char *look() const { return Pointer; }

    // \brief Returns a pointer to memory address at the point after where this
    // lexer is currently lexing.
    inline const char *peek() const { return Pointer + 1; }

    // \brief Consumes the current memory address.
    inline const char *consume() { return ++Pointer; }

    // \brief Forms a new token of the specified kind. The lexeme is a string
    // range from the current lexer's indicator position to the indicator's
    // position plus the len specified.
    Token form(tok::Type kind, std::size_t len = 1);

    // \brief Scans a new string literal from this lexer's buffer.
    Token scanStringLiteral();

    // \brief Scans a new character literal from this lexer's buffer.
    Token scanCharacterLiteral();

public:

    // ctor.
    Lexer(TranslationUnit &TU, DiagnosticBuilder &DB)
         : Unit(TU), Builder(DB) {
        Start   = Unit.getMemoryBuffer().begin();
        End     = Unit.getMemoryBuffer().end();

        Pointer = Start;
    }

    // \brief Scans the next token from this Lexer's buffer.
    Token scan();

};

} // namespace kaiju

#endif // KAIJU_PARSE_LEXER_H
