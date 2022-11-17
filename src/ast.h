#ifndef AST_H
#define AST_H

#include "stb_ds.h"

enum Decl_Type {
    DECL_NONE,
    DECL_FUNCTION,
    DECL_ENUM,
    DECL_STRUCT,
    DECL_UNION,
    DECL_VARIABLE,
};

struct Typespec {
    
};


struct FuncParam {
    char *identifier;
    Typespec *type;
};

struct Aggregate_Field {
    char *identifier;
    Typespec *type;
    Expr *expr;
};

struct Func_Decl {
    FuncParam *params;
};

struct Enum_Field {
    char *identifier;
    Expr *expr;
};

struct Enum_Decl {
    Enum_Field *fields;
};

struct Struct_Decl {
    Aggregate_Field *fields;
};

struct Union_Decl {
    Aggregate_Field *fields;
};

struct Var_Decl {
    char *identifier;
    Typespec *type;
    Expr *expr;
};

struct Decl {
    Decl_Type type;
    char *identifier;
    union {
        Func_Decl funcdecl;
        Enum_Decl *enumdecl;
        Struct_Decl *structdecl;
        Union_Decl *uniondecl;
        Var_Decl *vardecl;
    };
};

enum Stmt_Type {
    STMT_NONE,
    STMT_IF,
    STMT_DO,
    STMT_WHILE,
    STMT_FOR,
    STMT_SWITCH,
    STMT_RETURN,
    STMT_BREAK,
    STMT_CONTINUE,
    STMT_ASSIGN,
    STMT_AUTO_ASSIGN,
    STMT_EXPR,
    STMT_BLOCK,
};

struct Stmt {
    Stmt_Type type;
    Expr *expr;
    union {
        Stmt *statements;
        Expr *rhs;
    };
};

enum Expr_Type {
    EXPR_NONE,
    EXPR_INT,
    EXPR_FLOAT,
    EXPR_STR,
    EXPR_IDENT,
    EXPR_CAST,
    EXPR_CALL,
    EXPR_INDEX,
    EXPR_FIELD,
    EXPR_COMPOUND,
    EXPR_UNARY,
    EXPR_BINARY,
    EXPR_TERNARY,
};

struct Expr {
    Expr_Type type;
    
    union {
        uint64_t int_val;
        double float_val;
        char *str_val;
        char *identifier;
        
        struct {
            Typespec *compund_type;
            Expr **compound_list;
        };
        // unary
        struct {
            Expr *operand;
            union {
                Expr **args;
                Expr *index;
                const char *field;
            };
        };
        // binary
        struct {
            Expr *left;
            Expr *right;
        };
        // ternary
        struct {
            Expr *cond;
            Expr *then_expr;
            Expr *else_expr;
        };
    };
};
#endif
