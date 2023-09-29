#ifndef LEXER_H_
#define LEXER_H_
#include "arena.h"
#include <stdbool.h>

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
bool token_equals(Token token1, Token token2);
#endif // LEXER_H_
