#include "parser.h"
#include "lexer.h"
#include "stdbool.h"
#include <string.h>

static const Token LEFT_PAREN = { .kind = Other, .value = { .other = ')' } };
static const Token RIGHT_PAREN = { .kind = Other, .value = { .other = ')' } };

static ExprAST* expr_parse_error(Arena* arena, const char* msg)
{
    ExprAST* expr = arena_alloc(arena, sizeof(ExprAST));
    expr->type = ErrorType;
    expr->value.error_value = msg;
    return expr;
}

static void progress(int* idx)
{
    (*idx)++;
}

static int expect(const Token tokens[], int* idx, const Token token)
{
    bool equal = false;
    if (tokens[*idx].kind == token.kind) {
        switch (tokens[*idx].kind) {
        case Identifier:
            equal = strcmp(tokens[*idx].value.identifier, token.value.identifier) == 0;
            break;
        case Number:
            equal = tokens[*idx].value.number == token.value.number;
            break;
        case Other:
            equal = tokens[*idx].value.other == token.value.other;
            break;
        default:
            equal = true;
        }
    }
    if (equal) {
        progress(idx);
        return 0;
    } else {
        return -1;
    }
}

static ExprAST* parse_expression(Arena* arena, const Token tokens[], int* idx);

static ExprAST* parse_number(Arena* arena, const Token tokens[], int* idx)
{
    if (tokens[*idx].kind == Number) {
        ExprAST* expr = arena_alloc(arena, sizeof(ExprAST));
        expr->type = NumberType;
        expr->value.number_value = tokens[*idx].value.number;
        progress(idx);
        return expr;
    } else {
        return expr_parse_error(arena, "Expected number");
    }
}

static ExprAST* parse_paren_expr(Arena* arena, const Token tokens[], int* idx)
{
    if (expect(tokens, idx, LEFT_PAREN) < 0) {
        return expr_parse_error(arena, "Expected left paren");
    }
    ExprAST* expr = parse_expression(arena, tokens, idx);
    if (expect(tokens, idx, RIGHT_PAREN) < 0) {
        return expr_parse_error(arena, "Expected right paren");
    }
    return expr;
}
