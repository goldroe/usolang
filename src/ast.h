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
    Typespec type;
};

struct Func_Decl {
    FuncParam *params;
};

struct Enum_Decl {

};

struct Struct_Decl {

};
struct Union_Decl {

};
struct Var_Decl {
    
};

struct Decl {
    Decl_Type type;
    char *identifier;
    union {
        Func_Decl funcdecl;
        Enum_Decl enumdecl;
        Struct_Decl structdecl;
        Union_Decl uniondecl;
        Var_Decl vardecl;
    };
};

enum Stmt_Type {
    STMT_NONE,
    STMT_IF,
    STMT_WHILE,
    STMT_FOR,
    STMT_ASSIGNMENT,
    STMT_CALL,
    STMT_BLOCK,
};

enum Expr_Type {
    EXPR_NONE,
    EXPR_BINARY,
    EXPR_TYPECAST,
    EXPR_UNARY,
    EXPR_OPERAND,
    EXPR_CALL,
};

#endif
