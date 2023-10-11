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
    case TokEof:
        return "Eof";
    case TokDef:
        return "Def";
    case TokExtern:
        return "Extern";
    case TokIdentifier:
        return token.value.identifier;
    case TokNumber: {
        char* str = arena_alloc(arena, 20 * sizeof(char)); // Assuming max number length is 20
        snprintf(str, 20, "%lf", token.value.number);
        return str;
    }
    case TokOther: {
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
    case TokEof:
    case TokDef:
    case TokExtern:
        return true;
    case TokIdentifier:
        return strcmp(token1.value.identifier, token2.value.identifier) == 0;
    case TokNumber:
        return token1.value.number == token2.value.number;
    case TokOther:
        return token1.value.other == token2.value.other;
    default:
        return false;
    }
}

static int skip_whitespace(const char* input, size_t idx)
{
    while (input[idx] && isspace(input[idx])) {
        idx++;
    }
    return idx;
}

static int skip_comment(const char* input, size_t idx)
{
    while (input[idx] && input[idx] != '\n' && input[idx] != '\r') {
        idx++;
    }
    return idx;
}

static int lex_keyword_or_id(Arena* arena, const char* input, size_t idx, Token tokens[], int tokenCount)
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
        tokens[tokenCount].kind = TokDef;
    } else if (strcmp(identifierStr, "extern") == 0) {
        tokens[tokenCount].kind = TokExtern;
    } else {
        tokens[tokenCount].kind = TokIdentifier;
        tokens[tokenCount].value.identifier = identifierStr;
    }

    return idx;
}

static int lex_number(const char* input, size_t idx, Token tokens[], int tokenCount)
{
    const int start = idx;
    while (isdigit(input[idx]) || input[idx] == '.') {
        idx++;
    }
    const int length = idx - start;
    char* numStr = malloc(length + 1);
    strncpy(numStr, input + start, length);
    numStr[length] = '\0';
    tokens[tokenCount].kind = TokNumber;
    tokens[tokenCount].value.number = atof(numStr);
    free(numStr);

    return idx;
}

Token* lex(Arena* arena, const char* input)
{
    size_t idx = 0;
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
            tokens[tokenCount].kind = TokOther;
            tokens[tokenCount].value.other = input[idx];
            tokenCount++;
            idx++;
        }
    }

    tokens[tokenCount].kind = TokEof;
    return tokens;
}
