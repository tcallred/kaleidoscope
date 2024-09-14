#pragma once

#include "types.h"
#include <vector>

struct Expr {
    enum class Tag {
        Number, Variable, Binop, Call
    };
    Tag tag;
    usize token;
    usize lhs;
    usize rhs;
};

struct ExprAST {
    std::vector<Expr> exprs;
    usize root;
};

// typedef enum {
//     ExprNumberType,
//     ExprVariableType,
//     ExprBinopType,
//     ExprCallType
// } ExprType;

// typedef struct ExprAST {
//     ExprType type;
//     union {
//         const char* errorValue;
//         double numberValue;
//         const char* variableName;
//         struct {
//             char op;
//             struct ExprAST* lhs;
//             struct ExprAST* rhs;
//         } binop;
//         struct {
//             const char* callee;
//             struct ExprAST** args;
//             usize argsCount;
//         } call;
//     } value;
// } ExprAST;

// typedef struct {
//     const char* name;
//     const char** args;
//     usize argsCount;
// } PrototypeAST;

// typedef struct {
//     PrototypeAST proto;
//     ExprAST* body;
// } FunctionAST;
