#include "lexer.h"
#include "arena.h"
#include <ctype.h>
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

static int lex_keyword_or_id(const char* input, int idx, Token tokens[], const int token_count)
{

    const int start = idx;
    while (isalnum(input[idx])) {
        idx++;
    }
    const int length = idx - start;
    char* identifier_str = malloc(length + 1);
    strncpy(identifier_str, input + start, length);
    identifier_str[length] = '\0';
    if (strcmp(identifier_str, "def") == 0) {
        tokens[token_count].kind = Def;
    } else if (strcmp(identifier_str, "extern") == 0) {
        tokens[token_count].kind = Extern;
    } else {
        tokens[token_count].kind = Identifier;
        tokens[token_count].value.identifier = identifier_str;
    }

    return idx;
}

static int lex_number(const char* input, int idx, Token tokens[], const int token_count)
{
    const int start = idx;
    while (isdigit(input[idx]) || input[idx] == '.') {
        idx++;
    }
    const int length = idx - start;
    char* num_str = malloc(length + 1);
    strncpy(num_str, input + start, length);
    num_str[length] = '\0';
    tokens[token_count].kind = Number;
    tokens[token_count].value.number = atof(num_str);
    free(num_str);

    return idx;
}

Token* lex(Arena* arena, const char* input)
{
    int idx = 0;
    Token* tokens = arena_alloc(arena, sizeof(Token) * strlen(input));
    int token_count = 0;

    while (input[idx]) {
        idx = skip_whitespace(input, idx);
        if (!input[idx]) {
            break;
        }
        if (isalpha(input[idx])) {
            idx = lex_keyword_or_id(input, idx, tokens, token_count);
            token_count++;
        } else if (isdigit(input[idx]) || input[idx] == '.') {
            idx = lex_number(input, idx, tokens, token_count);
            token_count++;
        } else if (input[idx] == '#') {
            idx = skip_comment(input, idx);
        } else {
            tokens[token_count].kind = Other;
            tokens[token_count].value.other = input[idx];
            token_count++;
            idx++;
        }
    }

    tokens[token_count].kind = Eof;
    return tokens;
}
