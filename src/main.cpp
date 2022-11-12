#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>

#include <vector>

void syntax_error(const char *err, ...) {
	static char buf[512];
	va_list args;
	va_start(args, err);
	vsnprintf(buf, 512, err, args);
	va_end(args);
	
	printf("SYNTAX ERROR: %s\n", buf);
	exit(0);
}

struct InternStr {
	size_t length;
	char *string;
};

std::vector<InternStr> interns;

char *intern_str_range(const char *start, const char *end) {
	size_t length = end - start;
	char *str = (char *)calloc(length+1, sizeof(char));
	for (size_t i = 0; i < length; i++) {
		str[i] = *start++;
	}
	str[length] = 0;
	
	
	for (auto it = interns.begin(); it != interns.end(); it++) {
		size_t cmp_len = (length > it->length) ? length : it->length;
		if (strncmp(str, it->string, cmp_len) == 0) {
			return it->string;
		}
	}
	
	InternStr intern;
	intern.string = str;
	intern.length = length;
	interns.push_back(intern);
	return intern.string;
}

enum Token_Type {
	TOKEN_EOF,
	
	TOKEN_IDENT = 128,
	TOKEN_INTLIT,
	TOKEN_FLOATLIT,
	TOKEN_STRLIT,
	TOKEN_CHARLIT,
};

enum Token_Mod {
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
    Token_Mod modifier;
};

const char *stream;
Token token;

char escapes[128];

void scan_float() {
    const char *start = stream;
    double result = 0;
    while (isdigit(*stream) || *stream == '.') {
        stream++;
    }
    if (*stream == 'f') {
        result = strtod(start, (char **)&stream);
        stream++;
    } else {
        syntax_error("Invalid character in float literal %c\n", *stream);
    }

    token.type = TOKEN_FLOATLIT;
    token.float_val = result;
}

void scan_int() {
    uint64_t result = 0;
    int base = 10;
    if (*stream == '0') {
        stream++;
        if (toupper(*stream) == 'B') {
            stream++;
            base = 2;
            while (isdigit(*stream)) {
                int digit = *stream - '0';
                if (*stream > '1') {
                    syntax_error("Invalid digit in binary literal, %c\n", *stream);
                }
                result = result * base + digit;
                stream++;
            }
            token.modifier = MOD_BINARY;
        } else if (*stream == '0') {
            stream++;
            base = 8;
            while (isdigit(*stream)) {
                int digit = *stream - '0';
                if (*stream > '7') {
                    syntax_error("Invalid digit in octal literal, %c\n", *stream);
                }
                result = result * base + digit;
                stream++;
            }
            token.modifier = MOD_OCTAL;
        } else if (toupper(*stream) == 'X') {
            stream++;
            base = 16;
            while (isalnum(*stream)) {
                int digit = (isdigit(*stream)) ? (*stream - '0') : (toupper(*stream) - 'A');
                if (toupper(*stream) > 'F') {
                    syntax_error("Invalid digit in hexadecimal literal, %c\n", *stream);
                }
                result = result * base + digit;
                stream++;
            }
            token.modifier = MOD_HEX;
        } 
    } else {
        while (isdigit(*stream)) {
            int digit = *stream - '0';
            result = result * base + digit;
            stream++;
        }
        token.modifier = MOD_DECIMAL;
    }

    token.type = TOKEN_INTLIT;
    token.int_val = result;
}


void next_token() {
begin:
	const char *start = stream;
	switch (*stream) {
    case ' ': case '\t': case '\r': case '\n':
        stream++;
		goto begin;
		
    case '0': case '1': case '2': case '3': case '4': case '5':
    case '6': case '7': case '8': case '9': {
        while (isdigit(*stream)) {
            stream++;
        }
        char c = *stream;
        stream = start;
        if (c == '.' || c == 'f') {
            scan_float();
        } else {
            scan_int();
        }
    } break;

    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
    case 'G': case 'H': case 'I': case 'J': case 'K': case 'L':
    case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
    case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
    case 'Y': case 'Z':
    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
    case 'g': case 'h': case 'i': case 'j': case 'k': case 'l':
    case 'm': case 'n': case 'o': case 'p': case 'q': case 'r': 
    case 's': case 't': case 'u': case 'v': case 'w': case 'x': 
    case 'y': case 'z': {
        while (isalnum(*stream) || *stream == '_') {
            stream++;
        }
        token.type = TOKEN_IDENT;
        token.identifier = intern_str_range(start, stream);
    }break;
		
    case '\'': {
        char val = 0;
        stream++;
        if (*stream == '\n' || *stream == '\t' || *stream == '\'') {
            syntax_error("Illegal character in char literal, %c\n", *stream);
        }
        // escape sequence
        else if (*stream == '\\') {
            stream++;
            if (escapes[*stream]) {
                val = escapes[*stream];
                stream++;
            } else {
                syntax_error("Unsupported escape character, %c\n", val);
            }
        } else {
            val = *stream;
            stream++;
        }
        if (*stream != '\'') {
            syntax_error("Expected character \', got %c\n", *stream);
        } else {
            stream++;
        }
        token.type = TOKEN_CHARLIT;
        token.char_val = val;
    } break;
		
    case '\"': {
        stream++;
        int size = 0;
        char *val = (char *)calloc(1, sizeof(char));
        while (*stream) {
            if (*stream == '\"'){
                stream++;
                break;
            }
				
            if (*stream == '\n' || *stream == '\t') {
                syntax_error("Illegal character in string literal %c\n", *stream);
            } 
            // escape sequence
            else if (*stream == '\\') {
                stream++;
                if (escapes[*stream]) {
                    val = (char *)realloc(val, size+1);
                    val[size++] = escapes[*stream];
                    stream++;
                } else {
                    syntax_error("Unsupported escape character, %c\n", *stream);
                }
            } else {
                val = (char *)realloc(val, size+1);
                val[size++] = *stream;
                stream++;
            }
        }
        val[size] = 0;
			
        token.type = TOKEN_STRLIT;
        token.str_val = val;
    } break;

    case '/': {
        stream++;
        if (*stream == '/') { // line comment
            stream++;
            while (*stream != 0 && *stream != '\n' && *stream != '\r') {
                stream++;
            }
            // whitespace will be skipped next token pass
        } else if (*stream == '*') { // block comment

        }

    } break;
		
    default:
		token.type = (Token_Type)*stream++;
	}
}

void init_lexer() {
	escapes['t'] = '\t';
	escapes['n'] = '\n';
	escapes['r'] = '\r';
	escapes['\\'] = '\\';
	escapes['\''] = '\'';
	escapes['\"'] = '\"';
}

void lexer_tests() {
	stream = "10 + 10";
	next_token();
	assert(token.type == TOKEN_INTLIT && token.int_val == 10);
	next_token();
	assert(token.type == '+');
	next_token();
	assert(token.type == TOKEN_INTLIT && token.int_val == 10);
	next_token();
	assert(token.type == TOKEN_EOF);
	
	stream = "foo_buzz = 10";
	next_token();
	assert(token.type == TOKEN_IDENT && (!strcmp(token.identifier, "foo_buzz")));
	next_token();
	assert(token.type == '=');
	next_token();
	assert(token.type == TOKEN_INTLIT && token.int_val == 10);
	next_token();
	assert(token.type == TOKEN_EOF);
	
	stream = "foo + 10 \'1\' \'\\t\' ";
	next_token();
	assert(token.type == TOKEN_IDENT && !strcmp(token.identifier, "foo"));
	next_token();
	assert(token.type == '+');
	next_token();
	assert(token.type == TOKEN_INTLIT && token.int_val == 10);
	next_token();
	assert(token.type == TOKEN_CHARLIT && token.char_val == '1');
	next_token();
	assert(token.type == TOKEN_CHARLIT && token.char_val == '\t');
	next_token();
	assert(token.type == TOKEN_EOF);
	
	stream = "x = 10 \"Cool string, blah blah\\n\"";
	next_token();
	assert(token.type == TOKEN_IDENT && !strcmp(token.identifier, "x"));
	next_token();
	assert(token.type == '=');
	next_token();
	assert(token.type == TOKEN_INTLIT && token.int_val == 10);
	next_token();
	assert(token.type == TOKEN_STRLIT && !strcmp(token.str_val, "Cool string, blah blah\n"));
	next_token();
	assert(token.type == TOKEN_EOF);
	
	stream = "foo = 100.12f buzz = 200f";
	next_token();
	assert(token.type == TOKEN_IDENT && !strcmp(token.identifier, "foo"));
	next_token();
	assert(token.type == '=');
	next_token();
	assert(token.type == TOKEN_FLOATLIT && token.float_val == 100.12);
	next_token();
	assert(token.type == TOKEN_IDENT && !strcmp(token.identifier, "buzz"));
	next_token();
	assert(token.type == '=');
	next_token();
	assert(token.type == TOKEN_FLOATLIT && token.float_val == 200.0);
	next_token();
	assert(token.type == TOKEN_EOF);
}

int main(int argc, char **argv) {
	init_lexer();
	lexer_tests();
	
	return 0;
}
