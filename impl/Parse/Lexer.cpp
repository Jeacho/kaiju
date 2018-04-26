
#include "kaiju/Parse/Lexer.h"

using namespace kaiju;

#include "kaiju/Parse/Token.h"

// \brief Forms a new token of the specified kind. The lexeme is a string
// range from the current lexer's indicator position to the indicator's
// position plus the len specified.
Token Lexer::form(tok::Type kind, std::size_t len) {
    StringRef excerpt(Pointer, len);

    for (int i = 0; i < len; i++)
        consume();

    return Token(kind, excerpt);
}

// \brief Scans a new string literal from the character stream.
Token Lexer::scanStringLiteral() {
    assert(*look() == '\"'
        && "invalid state for scanStringLiteral subroutine.");

    const char *start = consume();

    while (*look() != '\"') {
        if (look() == End) {
            Builder.create(err::lex_extranous_eof);
            Builder.flush();
        }

        if (*look() == '\0' || *look() == '\n' || *look() == '\r') {
            Builder.create(err::lex_unterminated_string_literal);
            Builder.flush();
            break;
        }

        consume();
    }

    StringRef text(start, look() - start);

    if (*look() == '\"')
        consume();

    return Token(tok::string_literal, text);
}

// \brief Scans a new character literal from this lexer's buffer.
Token Lexer::scanCharacterLiteral() {
    assert(*look() == '\''
        && "invalid state for scanCharacterLiteral subroutine.");

    // skip the \' character denoting the start of this literal.
    consume();

    if (*look() == '\\') {
        switch (*consume()) {
        case '\'': case '\"': case '\?':
        case '\\': case 'a': case 'b':
        case 'f': case 'n': case 'r':
        case 't': case 'v':
            break;

        default:
            Builder.create(err::lex_unknown_escape_sequence,
                StringRef(look() - 1, 2));
            Builder.flush();
        }

        consume();
    } else if (*look() == '\'') {
        Builder.create(err::lex_empty_character_constant);
        Builder.flush();
    }

    StringRef text(look(), 1);
    consume();

    if (*look() != '\'') {
        while (*look() != '\'' && *look() != '\0'
            && *look() != '\r' && *look() != '\n') {
            consume();
        }

        if (*look() == '\'') {
            Builder.create(err::lex_multiple_characters_in_character_literal);
            Builder.flush();
        } else {
            Builder.create(err::lex_unterminated_character_literal);
            Builder.flush();
        }
    }

    return Token(tok::character_literal, text);
}

// \brief Lexer subroutine for scanning identifiers.
Token Lexer::scanIdentifier() {
    
}

// \brief Scans the next token from this Lexer's buffer.
Token Lexer::scan() {

RESTART:
    switch (*look()) {
    case '\t': case '\v':
    case '\f': case ' ':
        consume();
        goto RESTART;

    case '~': return form(tok::tilde);
    case '@': return form(tok::at);
    case '#': return form(tok::hash);
    case '$': return form(tok::dollar);
    case '.': return form(tok::period);
    case ',': return form(tok::comma);
    case ':': return form(tok::colon);
    case '\n': case '\r':
    case ';':
        return form(tok::new_line);

    case '\"':
        return scanStringLiteral();

    case '\'':
        return scanCharacterLiteral();

    case '\0':
        return form(tok::eof, 0);

    default:
        Builder.create(err::lex_malformed_character, *look());
        Builder.flush();

        return form(tok::none, 1);
    }
}
