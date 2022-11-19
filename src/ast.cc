#include "ast.h"

#include <stdlib.h>
#include <stdint.h>

Typespec *typespec_init(Typespec_Kind kind) {
    Typespec *typespec = (Typespec *)calloc(1, sizeof(Typespec));
    typespec->kind = kind;
    return typespec;
}



Expr *expr_init(Expr_Type type) {
    Expr *expr = (Expr *)calloc(1, sizeof(Expr));
    expr->type = type;
    
    return expr;
}

Expr *expr_int(uint64_t val) {
    Expr *expr = expr_init(EXPR_INT);
    expr->int_val = val;
    return expr;
}
Expr *expr_float(double val) {
    Expr *expr = expr_init(EXPR_FLOAT);
    expr->float_val = val;
    return expr;
}
Expr *expr_str(const char *str) {
    Expr *expr = expr_init(EXPR_STR);
    expr->str_val = str;
    return expr;
}
Expr *expr_identifier(const char *ident) {
    Expr *expr = expr_init(EXPR_IDENT);
    expr->identifier = ident;
    return expr;
}
Expr *expr_compound(Typespec *typespec, Expr **args, size_t num_args) {
    Expr *expr = expr_init(EXPR_COMPOUND);
    expr->compound.type = typespec;
    expr->compound.args = args;
    expr->compound.num_args = num_args;
    return expr;
}
Expr *expr_field(const char *identifier, const char *field) {
    Expr *expr = expr_init(EXPR_FIELD);
    expr->field.identifier = identifier;
    expr->field.field = field;
    return expr;
}
Expr *expr_index(const char *identifier, Expr *index_expr) {
    Expr *expr = expr_init(EXPR_INDEX);
    expr->index.identifier = identifier;
    expr->index.expr = index_expr;
    return expr;
}
Expr *expr_call(const char *identifier, Typespec **ret_types, size_t num_ret, Expr **args, size_t num_args) {
    Expr *expr = expr_init(EXPR_CALL);
    expr->call.identifier = identifier;
    expr->call.ret_types = ret_types;
    expr->call.num_ret = num_ret;
    expr->call.args = args;
    expr->call.num_args = num_args;
    return expr;
};
Expr *expr_cast(Typespec *type, Expr *cast_expr) {
    Expr *expr = expr_init(EXPR_CAST);
    expr->cast.type = type;
    expr->cast.expr = cast_expr;
    return expr;
}
Expr *expr_unary(Token_Type op, Expr *unary_expr) {
    Expr *expr = expr_init(EXPR_UNARY);
    expr->unary.op = op;
    expr->unary.expr = unary_expr;
    return expr;
}
Expr *expr_binary(Expr *left, Expr *right) {
    Expr *expr = expr_init(EXPR_BINARY);
    expr->binary.left = left;
    expr->binary.right = right;
    return expr;
}
Expr *expr_ternary(Expr *cond, Expr *then_expr, Expr *else_expr) {
    Expr *expr = expr_init(EXPR_TERNARY);
    expr->ternary.cond = cond;
    expr->ternary.then_expr = then_expr;
    expr->ternary.else_expr = else_expr;
    return expr;
}
