#ifndef AST_H
#define AST_H

#include <stdint.h>

enum Typespec_Kind {
    TYPESPEC_NONE,
    TYPESPEC_IDENT,
    TYPESPEC_ARRAY,
    TYPESPEC_POINTER,
    TYPESPEC_FUNCTION,
};

struct Typespec {
    Typespec_Kind kind;

    union {
        const char *identifier;
    };
};

enum Expr_Type {
    EXPR_NONE,
    EXPR_UNARY,
    EXPR_BINARY,
    EXPR_TERNARY,
    EXPR_CALL,
    EXPR_INDEX,
    EXPR_FIELD,
    EXPR_INT,
    EXPR_FLOAT,
    EXPR_STR,
    EXPR_IDENT,
    EXPR_COMPOUND,
    EXPR_CAST,
};

struct Compound_Expr {
    Typespec *type;
    Expr **args;
    size_t num_args;
};
struct Field_Expr {
    const char *identifier;
    const char *field;
};
struct Index_Expr {
    const char *identifier;
    Expr *expr;
};
struct Call_Expr {
    const char *identifier;
    Typespec *return_type;
    Expr **args;
    size_t num_args;
};
struct Cast_Expr {
    Typespec *type;
    Expr *expr;
};
struct Unary_Expr {
    Token_Type *op;
    Expr *expr;
};
struct Binary_Expr {
    Expr *left;
    Expr *right;
};
struct Ternary_Expr {
    Expr *cond;
    Expr *then_expr;
    Expr *else_expr;
};

struct Expr {
    Expr_Type type;

    union {
        uint64_t int_val;
        double float_val;
        const char *str_val;
        const char *identifier;

        Compound_Expr compound;
        Field_Expr field;
        Index_Expr index;
        Call_Expr call;
        Cast_Expr cast;
        Unary_Expr unary;
        Binary_Expr binary;
        Ternary_Expr ternary;
    };
};

#endif
