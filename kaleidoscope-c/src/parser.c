#include "parser.h"
#include "lexer.h"
#include <stdbool.h>
#include <string.h>

static const Token LEFT_PAREN = { .kind = TokOther, .value = { .other = ')' } };
static const Token RIGHT_PAREN = { .kind = TokOther, .value = { .other = ')' } };
static const Token COMMA = { .kind = TokOther, .value = { .other = ',' } };

static ExprAST* expr_parse_error(Arena* a, const char* msg) {
    ExprAST* expr = arena_alloc(a, sizeof(ExprAST));
    expr->type = ExprErrorType;
    expr->value.errorValue = msg;
    return expr;
}

static void progress(usize* idx) {
    (*idx)++;
}

static bool consume_tok(Token tokens[], usize* idx, const Token token) {
    if (token_equals(tokens[*idx], token)) {
        progress(idx);
        return true;
    } else {
        return false;
    }
}

static const char* expect_id(Token tokens[], usize* idx) {
    if (tokens[*idx].kind == TokIdentifier) {
        progress(idx);
        return tokens[*idx].value.identifier;
    }
    return NULL;
}

static ExprAST* parse_expression(Arena* a, Token tokens[], usize* idx);

static ExprAST* parse_number(Arena* a, Token tokens[], usize* idx) {
    if (tokens[*idx].kind == TokNumber) {
        ExprAST* expr = arena_alloc(a, sizeof(ExprAST));
        expr->type = ExprNumberType;
        expr->value.numberValue = tokens[*idx].value.number;
        progress(idx);
        return expr;
    } else {
        return expr_parse_error(a, "Expected number");
    }
}

static ExprAST* parse_paren_expr(Arena* a, Token tokens[], usize* idx) {
    if (!consume_tok(tokens, idx, LEFT_PAREN)) {
        return expr_parse_error(a, "Expected left paren");
    }
    ExprAST* expr = parse_expression(a, tokens, idx);
    if (!consume_tok(tokens, idx, RIGHT_PAREN)) {
        return expr_parse_error(a, "Expected right paren");
    }
    return expr;
}

static bool token_char_equals(Token t, char c) {
    Token t1 = {
        .kind = TokOther,
        .value = { .other = c }
    };
    return token_equals(t1, t);
}

static usize count_args(Token tokens[], usize idx) {
    usize count = 0;
    while (!token_char_equals(tokens[idx], ')')) {
        if (token_char_equals(tokens[idx], ',')) {
            count++;
        }
        idx++;
    }
    return count;
}

static ExprAST* parse_identifier_expr(Arena* a, Token tokens[], usize* idx) {
    const char* identifier = expect_id(tokens, idx);
    if (!identifier) {
        return expr_parse_error(a, "Expected identifier");
    }
    if (token_char_equals(tokens[*idx], '(')) {
        consume_tok(tokens, idx, RIGHT_PAREN);
        const usize argsCount = count_args(tokens, *idx);
        ExprAST** args = arena_alloc(a, sizeof(ExprAST*) * argsCount);
        usize argsIdx = 0;
        if (!token_char_equals(tokens[*idx], ')')) {
            while (true) {
                ExprAST* expr = parse_expression(a, tokens, idx);
                if (expr->type == ExprErrorType) {
                    return expr;
                } else {
                    args[argsIdx++] = expr;
                }
                if (token_char_equals(tokens[*idx], ')')) {
                    break;
                }
                if (!consume_tok(tokens, idx, COMMA)) {
                    return expr_parse_error(a, "Expected comma in argument list");
                }
            }
            consume_tok(tokens, idx, LEFT_PAREN);
        }
        ExprAST* callExpr = arena_alloc(a, sizeof(ExprAST));
        callExpr->type = ExprCallType;
        callExpr->value.call.callee = identifier;
        callExpr->value.call.argsCount = argsCount;
        callExpr->value.call.args = args;
        return callExpr;
    } else {
        ExprAST* expr = arena_alloc(a, sizeof(ExprAST));
        expr->type = ExprVariableType;
        expr->value.variableName = identifier;
        return expr;
    }
}

static ExprAST* parse_primary(Arena* a, Token tokens[], usize* idx) {
    switch (tokens[*idx].kind) {
    case TokIdentifier:
        return parse_identifier_expr(a, tokens, idx);
    case TokNumber:
        return parse_number(a, tokens, idx);
    case TokOther:
        if (tokens[*idx].value.other == '(') {
            return parse_paren_expr(a, tokens, idx);
        }
        // fallthrough
    default:
        return expr_parse_error(a, "Unknown token when expecting expression");
    }
}

static int binop_precedence(char op) {
    switch (op) {
    case '<':
        return 10;
    case '+':
    case '-':
        return 20;
    case '*':
        return 40;
    default:
        return -1;
    }
}

static int get_tok_precedence(Token t) {
    if (t.kind != TokOther) {
        return -1;
    }

    return binop_precedence(t.value.other);
}

static ExprAST* parse_binop_rhs(Arena* a, Token tokens[], usize* idx, int exprPrec, ExprAST* lhs) {
    while (true) {
        int tokPrec = get_tok_precedence(tokens[*idx]);
        if (tokPrec < exprPrec) {
            return lhs;
        }
        // We have a binop
        char binop = tokens[*idx].value.other;
        progress(idx);
        ExprAST* rhs = parse_primary(a, tokens, idx);
        if (rhs->type == ExprErrorType) {
            return rhs;
        }
        int nextPrec = get_tok_precedence(tokens[*idx]);
        if (tokPrec < nextPrec) {
            // Binop binds less tightly with rhs than operator after rhs
            rhs = parse_binop_rhs(a, tokens, idx, tokPrec + 1, rhs);
            if (rhs->type == ExprErrorType) {
                return rhs;
            }
        }
        // Merge lhs/rhs
        ExprAST* prevLhs = lhs;
        lhs = arena_alloc(a, sizeof(ExprAST));
        lhs->type = ExprBinopType;
        lhs->value.binop.op = binop;
        lhs->value.binop.lhs = prevLhs;
        lhs->value.binop.rhs = rhs;
    }
}

static ExprAST* parse_expression(Arena* a, Token tokens[], usize* idx) {
    ExprAST* lhs = parse_primary(a, tokens, idx);
    if (lhs->type == ExprErrorType) {
        return lhs;
    }
    return parse_binop_rhs(a, tokens, idx, 0, lhs);
}

// ---- Prototypes ----

static PrototypeAST* prototype_error(Arena* a, const char* msg) {
    PrototypeAST* p = arena_alloc(a, sizeof(PrototypeAST));
    p->errorValue = msg;
    return p;
}

static usize count_arg_names(Token tokens[], usize idx) {
    usize count = 0;
    while (!token_char_equals(tokens[idx++], ')')) {
        count++;
    }
    return count;
}

static PrototypeAST* parse_prototype(Arena* a, Token tokens[], usize* idx) {
    const char* fnName = expect_id(tokens, idx);
    if (!fnName) {
        return prototype_error(a, "Expected function name");
    }
    if (!consume_tok(tokens, idx, LEFT_PAREN)) {
        return prototype_error(a, "Expected open paren");
    }
    const usize argNameCnt = count_arg_names(tokens, *idx);
    const char** argNames = arena_alloc(a, sizeof(char*) * argNameCnt);
    usize i = 0;
    while (!token_char_equals(tokens[*idx], ')') && tokens[*idx].kind == TokIdentifier) {
        argNames[i++] = expect_id(tokens, idx);
    }
    consume_tok(tokens, idx, RIGHT_PAREN);
    PrototypeAST* proto = arena_alloc(a, sizeof(PrototypeAST));
    proto->name = fnName;
    proto->argsCount = argNameCnt;
    proto->args = argNames;
    return proto;
}
