
#ifndef KAIJU_PARSE_TOKEN_H
#define KAIJU_PARSE_TOKEN_H

#include "kaiju/ADT/StringRef.h"
#include "kaiju/IO/SourceLoc.h"

namespace kaiju {

namespace tok {

// \brief The a token identifier paired with lexemes to create tokens.
enum Type {
    none = 0,
#define KEYWORD(name, str)      kw_ ## name,
#define OPERATOR(name, str)     name,
#define MISC(name, str)         name,
#define LITERAL(name, str)      name,
#define PUNCTUATOR(name, str)   name,
#include "kaiju/Parse/TokenTypes.def"
};

} // namespace tok

// Class Token
//
// \brief The Kaiju token interface.
//
class Token {

    // The kind of token this is.
    tok::Type Kind;

    // This lexeme that represents this token
    StringRef text;

public:
    Token(tok::Type type, StringRef str)
        : Kind(type), text(str) { /* empty */ }

    // \brief Returns a SourceLoc from the beginning of this tokens lexeme.
    inline SourceLoc getLoc() const { return text.begin(); }

    // \brief Returns this token's type.
    inline tok::Type getKind() const { return Kind; }

    // \brief Returns this tokens lexeme.
    inline StringRef getText() const { return text; }

    // \brief The following methods are for convientently comparing token types.

    bool is(tok::Type K)    const { return Kind == K; }
    bool isNot(tok::Type K) const { return Kind != K; }

    template <typename... Ts>
    bool is(tok::Type K,     Ts... Ks) const {
        return is(K) || is(Ks...);
    }

    template <typename... Ts>
    bool isNot(tok::Type K, Ts... Ks) const {
        return isNot(K) && isNot(Ks...);
    }
};

std::ostream &operator<<(std::ostream &os, const Token &tok);

} // namespace kaiju

#endif // KAIJU_PARSE_TOKEN_H
