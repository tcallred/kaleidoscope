#include "parser.h"
#include "lexer.h"
#include <stdbool.h>
#include <string.h>

static const Token LEFT_PAREN = { .kind = Other, .value = { .other = ')' } };
static const Token RIGHT_PAREN = { .kind = Other, .value = { .other = ')' } };
static const Token COMMA = { .kind = Other, .value = { .other = ',' } };

static ExprAST* expr_parse_error(Arena* arena, const char* msg)
{
    ExprAST* expr = arena_alloc(arena, sizeof(ExprAST));
    expr->type = ErrorType;
    expr->value.errorValue = msg;
    return expr;
}

static void progress(int* idx)
{
    (*idx)++;
}

static bool consume_tok(const Token tokens[], int* idx, const Token token)
{
    if (token_equals(tokens[*idx], token)) {
        progress(idx);
        return true;
    } else {
        return false;
    }
}

static const char* expect_id(const Token tokens[], int* idx)
{
    if (tokens[*idx].kind == Identifier) {
        progress(idx);
        return tokens[*idx].value.identifier;
    }
    return NULL;
}

extern ExprAST* parse_expression(Arena* arena, const Token tokens[], int* idx);

static ExprAST* parse_number(Arena* arena, const Token tokens[], int* idx)
{
    if (tokens[*idx].kind == Number) {
        ExprAST* expr = arena_alloc(arena, sizeof(ExprAST));
        expr->type = NumberType;
        expr->value.numberValue = tokens[*idx].value.number;
        progress(idx);
        return expr;
    } else {
        return expr_parse_error(arena, "Expected number");
    }
}

static ExprAST* parse_paren_expr(Arena* arena, const Token tokens[], int* idx)
{
    if (consume_tok(tokens, idx, LEFT_PAREN)) {
        return expr_parse_error(arena, "Expected left paren");
    }
    ExprAST* expr = parse_expression(arena, tokens, idx);
    if (consume_tok(tokens, idx, RIGHT_PAREN)) {
        return expr_parse_error(arena, "Expected right paren");
    }
    return expr;
}

static bool token_char_equals(Token t, char c)
{
    Token t1 = {
        .kind = Other,
        .value = { .other = c }
    };
    return token_equals(t1, t);
}

static size_t count_args(const Token tokens[], int idx)
{
    size_t count = 0;
    while (!token_char_equals(tokens[idx], ')')) {
        if (token_char_equals(tokens[idx], ',')) {
            count++;
        }
        idx++;
    }
    return count;
}

static ExprAST* parse_identifier_expr(Arena* arena, const Token tokens[], int* idx)
{
    const char* identifier = expect_id(tokens, idx);
    if (!identifier) {
        return expr_parse_error(arena, "Expected identifier");
    }
    if (token_char_equals(tokens[*idx], '(')) {
        consume_tok(tokens, idx, RIGHT_PAREN);
        const size_t argsCount = count_args(tokens, *idx);
        ExprAST** args = arena_alloc(arena, sizeof(ExprAST*) * argsCount);
        size_t argsIdx = 0;
        if (!token_char_equals(tokens[*idx], ')')) {
            while (true) {
                ExprAST* expr = parse_expression(arena, tokens, idx);
                if (expr->type == ErrorType) {
                    return expr;
                } else {
                    args[argsIdx++] = expr;
                }
                if (token_char_equals(tokens[*idx], ')')) {
                    break;
                }
                if (consume_tok(tokens, idx, COMMA)) {
                    return expr_parse_error(arena, "Expected comma in argument list");
                }
            }
            consume_tok(tokens, idx, LEFT_PAREN);
        }
        ExprAST* callExpr = arena_alloc(arena, sizeof(ExprAST));
        callExpr->type = CallType;
        callExpr->value.call.callee = identifier;
        callExpr->value.call.argsCount = argsCount;
        callExpr->value.call.args = args;
        return callExpr;
    } else {
        ExprAST* expr = arena_alloc(arena, sizeof(ExprAST));
        expr->type = VariableType;
        expr->value.variableName = identifier;
        return expr;
    }
}
