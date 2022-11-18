#include "common.h"
#include "lexer.h"
#include "tests.h"

#include <assert.h>

extern const char *stream;
extern Token token;

void init_stream(const char *str) {
    stream = str;
    next_token();
}

#define assert_token_ident(value) (assert(token.identifier == intern_str(value) && match_token(TOKEN_IDENT)))
#define assert_token_int(value) (assert(token.int_val == value && match_token(TOKEN_INTLIT)))
#define assert_token_float(value) (assert(token.float_val == value && match_token(TOKEN_FLOATLIT)))
#define assert_token_char(value) (assert(token.char_val == value && match_token(TOKEN_CHARLIT)))
#define assert_token_str(value) (assert(token.str_val == intern_str(value) && match_token(TOKEN_STRLIT)))
#define assert_token_type(type) (assert(match_token((Token_Type)type)))
#define assert_token_eof() (assert(is_token(TOKEN_EOF)))

void lexer_tests() {
    init_stream("10 + 10");
    assert_token_int(10);
    assert_token_type('+');
    assert_token_int(10);
    assert_token_eof();

    init_stream("0xFF + 0B10 = 257; 040");
    assert_token_int(255);
    assert_token_type('+');
    assert_token_int(2);
    assert_token_type('=');
    assert_token_int(257);
    assert_token_type(';');
    assert_token_int(32);
    assert_token_eof();
	
    init_stream("foo_buzz = 10");
    assert_token_ident("foo_buzz");
    assert_token_type('=');
    assert_token_int(10);
    assert_token_eof();
	
    init_stream("foo + 10 \'1\' \'\\t\' ");
    assert_token_ident("foo");
    assert_token_type('+');
    assert_token_int(10);
    assert_token_char('1');
    assert_token_char('\t');
    assert_token_eof();
	
    init_stream("x = 10 \"Cool string, blah blah\\n\"");
    assert_token_ident("x");
    assert_token_type('=');
    assert_token_int(10);
    assert_token_str("Cool string, blah blah\n");
    assert_token_eof();

    init_stream("foo = 100.12f buzz = 200f");
    assert_token_ident("foo");
    assert_token_type('=');
    assert_token_float(100.12);
    assert_token_ident("buzz");
    assert_token_type('=');
    assert_token_float(200.0);
    assert_token_eof();

    init_stream("//inline comment.\r\n foo + buzz = 10");;
    assert_token_ident("foo");
    assert_token_type('+');
    assert_token_ident("buzz");
    assert_token_type('=');
    assert_token_int(10);

    init_stream("/* *block comment* \n********/ 10 + 0x10\n");
    assert_token_int(10);
    assert_token_type('+');
    assert_token_int(16);
    assert_token_eof();
}

void parse_tests() {
    
}
