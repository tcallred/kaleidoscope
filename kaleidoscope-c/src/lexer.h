#ifndef LEXER_H_
#define LEXER_H_
#include "arena.h"

typedef enum {
    Eof,
    Def,
    Extern,
    Identifier,
    Number,
    Other
} TokenKind;

typedef struct
{
    TokenKind kind;
    union {
        char* identifier;
        double number;
        char other;
    } value;
} Token;

extern Token* lex(Arena* arena, const char* input);
const char* token_to_string(Arena* arena, Token token);
#endif // LEXER_H_
