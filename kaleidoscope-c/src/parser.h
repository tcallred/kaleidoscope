#pragma once

#include "arena.h"
#include "types.h"

typedef enum {
    ExprNumberType,
    ExprVariableType,
    ExprBinopType,
    ExprCallType
} ExprType;

typedef struct ExprAST {
    ExprType type;
    union {
        const char* errorValue;
        double numberValue;
        const char* variableName;
        struct {
            char op;
            struct ExprAST* lhs;
            struct ExprAST* rhs;
        } binop;
        struct {
            const char* callee;
            struct ExprAST** args;
            usize argsCount;
        } call;
    } value;
} ExprAST;

typedef struct {
    const char* name;
    const char** args;
    usize argsCount;
} PrototypeAST;

typedef struct {
    PrototypeAST proto;
    ExprAST* body;
} FunctionAST;
