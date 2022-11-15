#ifndef LEXER_H
#define LEXER_H

#include <stdint.h>

enum Token_Type {
	TOKEN_EOF,
	
	TOKEN_IDENT = 128,
	TOKEN_INTLIT,
	TOKEN_FLOATLIT,
	TOKEN_STRLIT,
	TOKEN_CHARLIT,

    TOKEN_INC,
    TOKEN_DEC,
    
    TOKEN_LSHIFT,
    TOKEN_RSHIFT,
    TOKEN_OR,
    TOKEN_AND,
    
    TOKEN_LTEQ,
    TOKEN_GTEQ,
    TOKEN_EQ,
    TOKEN_NEQ,

    TOKEN_MUL_ASSIGN,
    TOKEN_ADD_ASSIGN,
    TOKEN_SUB_ASSIGN,
    TOKEN_DIV_ASSIGN,
    TOKEN_MOD_ASSIGN,
    TOKEN_COLON_ASSIGN,
    TOKEN_LSHIFT_ASSIGN,
    TOKEN_RSHIFT_ASSIGN,
    TOKEN_OR_ASSIGN,
    TOKEN_XOR_ASSIGN,
    TOKEN_AND_ASSIGN,
    TOKEN_NOT_ASSIGN,
};

enum Token_Mod {
    MOD_NONE,
    
    MOD_HEX,
    MOD_OCTAL,
    MOD_BINARY,
    MOD_DECIMAL,
};

struct Token {
	Token_Type type;
	union {
		uint64_t int_val;
		double float_val;
		char *identifier;
		char *str_val;
		char char_val;
	};
    Token_Mod mod;
};

extern Token token;

void init_lexer();
void next_token();
bool is_token(Token_Type type);
bool is_token(char type);
bool match_token(Token_Type type);
bool match_token(char type);

void init_stream(const char *stream);

#endif
