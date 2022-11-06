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

char *range_to_str(const char *start, const char *end) {
	size_t size = end - start;
	char *str = (char *)calloc(size+1, sizeof(char));
	for (size_t i = 0; i < size; i++) {
		str[i] = *start++;
	}
	return str;
}

enum Token_Type {
	TOKEN_EOF,
	
	TOKEN_IDENT = 128,
	TOKEN_INTLIT,
	TOKEN_FLOATLIT,
	TOKEN_STRLIT,
	TOKEN_CHARLIT,
};

struct Token {
	Token_Type type;
	union {
		uint64_t intlit;
		double floatlit;
		char *ident;
		char *strlit;
		char charlit;
	};
};

const char *stream;
Token token;

char escapes[128];

double scan_float() {
	char *end = (char *)stream;
	
	while (isdigit(*end)) {
		end++;
	}
	if (*end == '.') {
		end++;
		while (isdigit(*end)) {
			end++;
		}
	} else if (*end == 'f') {
		end++;
	}
	
	double result = strtod(stream, &end);
	if (*end == 'f') {
		end++;
	}
	stream = end;
	return result;
}

uint64_t scan_int(int base) {
	uint64_t result = 0;
	while (isdigit(*stream)) {
		int digit = *stream - '0';
		result = result * base + digit;
		stream++;
	}
	return result;
}

void next_token() {
	begin:
	const char *start = stream;
	switch (*stream) {
		case ' ': case '\t': case '\r': case '\n':
		stream++;
		goto begin;
		
		case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': {
			uint64_t int_val = 0;
			double float_val = 0.0f;
			while (isdigit(*stream)) {
				stream++;
			}
			if (*stream == '.' || *stream == 'f') {
				stream = start;
				token.type = TOKEN_FLOATLIT;
				float_val = scan_float();
			} else {
				stream = start;
				if (*stream == '0') {
					stream++;
					token.type = TOKEN_INTLIT;
					
					if (toupper(*stream) == 'X') { // HEXADECIMAL
						stream++;
						token.intlit = scan_int(16);
					} else if (isdigit(*stream)) { // OCTAL (float conflict?)
						stream++;
						token.intlit = scan_int(8);
					} else if (toupper(*stream) == 'B') { // BINARY
						stream++;
						token.intlit = scan_int(2);
					} else {
						token.intlit = 0;
					}
				}
			}
		} break;
		
		case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
		case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u': case 'v': case 'w': case 'x': case 'y': case 'z': {
			while (isalnum(*stream) || *stream == '_') {
				stream++;
			}
			token.type = TOKEN_IDENT;
			token.ident = range_to_str(start, stream);
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
			token.charlit = val;
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
			token.strlit = val;
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

void lexer_test() {
	stream = "10 + 10";
	next_token();
	assert(token.type == TOKEN_INTLIT && token.intlit == 10);
	next_token();
	assert(token.type == '+');
	next_token();
	assert(token.type == TOKEN_INTLIT && token.intlit == 10);
	next_token();
	assert(token.type == TOKEN_EOF);
	
	stream = "foo_buzz = 10";
	next_token();
	assert(token.type == TOKEN_IDENT && (!strcmp(token.ident, "foo_buzz")));
	next_token();
	assert(token.type == '=');
	next_token();
	assert(token.type == TOKEN_INTLIT && token.intlit == 10);
	next_token();
	assert(token.type == TOKEN_EOF);
	
	stream = "foo + 10 \'1\' \'\\t\' ";
	next_token();
	assert(token.type == TOKEN_IDENT && !strcmp(token.ident, "foo"));
	next_token();
	assert(token.type == '+');
	next_token();
	assert(token.type == TOKEN_INTLIT && token.intlit == 10);
	next_token();
	assert(token.type == TOKEN_CHARLIT && token.charlit == '1');
	next_token();
	assert(token.type == TOKEN_CHARLIT && token.charlit == '\t');
	next_token();
	assert(token.type == TOKEN_EOF);
	
	stream = "x = 10 \"Cool string, blah blah\\n\"";
	next_token();
	assert(token.type == TOKEN_IDENT && !strcmp(token.ident, "x"));
	next_token();
	assert(token.type == '=');
	next_token();
	assert(token.type == TOKEN_INTLIT && token.intlit == 10);
	next_token();
	assert(token.type == TOKEN_STRLIT && !strcmp(token.strlit, "Cool string, blah blah\n"));
	next_token();
	assert(token.type == TOKEN_EOF);
	
	stream = "foo = 100.12 buzz = 200f";
	next_token();
	assert(token.type == TOKEN_IDENT && !strcmp(token.ident, "foo"));
	next_token();
	assert(token.type == '=');
	next_token();
	assert(token.type == TOKEN_FLOATLIT && token.floatlit == 100.12);
	next_token();
	assert(token.type == TOKEN_IDENT && !strcmp(token.ident, "buzz"));
	next_token();
	assert(token.type == '=');
	next_token();
	assert(token.type == TOKEN_FLOATLIT && token.floatlit == 200.f);
	next_token();
	assert(token.type == TOKEN_EOF);
}

int main(int argc, char **argv) {
	init_lexer();
	lexer_test();
	
	return 0;
}