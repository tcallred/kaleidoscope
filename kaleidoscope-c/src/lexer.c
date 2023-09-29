#include "lexer.h"
#include "arena.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* token_to_string(Arena* arena, Token token)
{
    switch (token.kind) {
    case Eof:
        return "Eof";
    case Def:
        return "Def";
    case Extern:
        return "Extern";
    case Identifier:
        return token.value.identifier;
    case Number: {
        char* str = arena_alloc(arena, 20 * sizeof(char)); // Assuming max number length is 20
        snprintf(str, 20, "%lf", token.value.number);
        return str;
    }
    case Other: {
        char* str = arena_alloc(arena, 2 * sizeof(char)); // Assuming other is a single character
        str[0] = token.value.other;
        str[1] = '\0';
        return str;
    }
    default:
        return "Unknown";
    }
}

bool token_equals(Token token1, Token token2)
{
    if (token1.kind != token2.kind) {
        return false;
    }

    switch (token1.kind) {
    case Eof:
    case Def:
    case Extern:
        return true;
    case Identifier:
        return strcmp(token1.value.identifier, token2.value.identifier) == 0;
    case Number:
        return token1.value.number == token2.value.number;
    case Other:
        return token1.value.other == token2.value.other;
    default:
        return false;
    }
}

static int skip_whitespace(const char* input, int idx)
{
    while (input[idx] && isspace(input[idx])) {
        idx++;
    }
    return idx;
}

static int skip_comment(const char* input, int idx)
{
    while (input[idx] && input[idx] != '\n' && input[idx] != '\r') {
        idx++;
    }
    return idx;
}

static int lex_keyword_or_id(Arena* arena, const char* input, int idx, Token tokens[], const int tokenCount)
{

    const int start = idx;
    while (isalnum(input[idx])) {
        idx++;
    }
    const int length = idx - start;
    char* identifierStr = arena_alloc(arena, length + 1);
    strncpy(identifierStr, input + start, length);
    identifierStr[length] = '\0';
    if (strcmp(identifierStr, "def") == 0) {
        tokens[tokenCount].kind = Def;
    } else if (strcmp(identifierStr, "extern") == 0) {
        tokens[tokenCount].kind = Extern;
    } else {
        tokens[tokenCount].kind = Identifier;
        tokens[tokenCount].value.identifier = identifierStr;
    }

    return idx;
}

static int lex_number(const char* input, int idx, Token tokens[], const int tokenCount)
{
    const int start = idx;
    while (isdigit(input[idx]) || input[idx] == '.') {
        idx++;
    }
    const int length = idx - start;
    char* numStr = malloc(length + 1);
    strncpy(numStr, input + start, length);
    numStr[length] = '\0';
    tokens[tokenCount].kind = Number;
    tokens[tokenCount].value.number = atof(numStr);
    free(numStr);

    return idx;
}

Token* lex(Arena* arena, const char* input)
{
    int idx = 0;
    Token* tokens = arena_alloc(arena, sizeof(Token) * strlen(input));
    int tokenCount = 0;

    while (input[idx]) {
        idx = skip_whitespace(input, idx);
        if (!input[idx]) {
            break;
        }
        if (isalpha(input[idx])) {
            idx = lex_keyword_or_id(arena, input, idx, tokens, tokenCount);
            tokenCount++;
        } else if (isdigit(input[idx]) || input[idx] == '.') {
            idx = lex_number(input, idx, tokens, tokenCount);
            tokenCount++;
        } else if (input[idx] == '#') {
            idx = skip_comment(input, idx);
        } else {
            tokens[tokenCount].kind = Other;
            tokens[tokenCount].value.other = input[idx];
            tokenCount++;
            idx++;
        }
    }

    tokens[tokenCount].kind = Eof;
    return tokens;
}
