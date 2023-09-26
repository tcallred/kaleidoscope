#ifndef PARSER_H_
#define PARSER_H_
#include "arena.h"

// Define a tagged union for ExprAST
typedef enum {
    ErrorType = 0,
    NumberType,
    VariableType,
    BinopType,
    CallType
} ExprType;

typedef struct ExprAST {
    ExprType type;
    union {
        const char* error_value;
        double number_value; // for NumberType
        char* variable_name; // for VariableType
        struct { // for BinopType
            char op;
            struct ExprAST* lhs;
            struct ExprAST* rhs;
        } binop;
        struct { // for CallType
            char* callee;
            struct ExprAST** args;
            size_t args_count;
        } call;
    } value;
} ExprAST;

// Define PrototypeAST in C
typedef struct {
    char* name;
    ExprAST** args;
    size_t args_count;
} PrototypeAST;

// Define FunctionAST in C
typedef struct {
    PrototypeAST proto;
    ExprAST* body;
} FunctionAST;

#endif
