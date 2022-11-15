#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#include "common.h"
#include "lexer.h"

#include "stb_ds.h"

void syntax_error(const char *err, ...) {
	static char buf[512];
	va_list args;
	va_start(args, err);
	vsnprintf(buf, 512, err, args);
	va_end(args);
    
	printf("SYNTAX ERROR: %s\n", buf);
	exit(0);
}

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


//@todo overflow checking
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
                if (digit > 1) {
                    syntax_error("Invalid digit in binary literal, %c\n", *stream);
                }
                result = result * base + digit;
                stream++;
            }
            token.mod = MOD_BINARY;
        } else if (toupper(*stream) == 'X') {
            stream++;
            base = 16;
            while (isalnum(*stream)) {
                int digit = (isdigit(*stream)) ? (*stream - '0') : (toupper(*stream) - 'A' + 10);
                if (digit > 15) {
                    syntax_error("Invalid digit in hexadecimal literal, %c\n", *stream);
                }
                result = result * base + digit;
                stream++;
            }
            token.mod = MOD_HEX;
        } else if (isdigit(*stream)) {
            base = 8;
            while (isdigit(*stream)) {
                int digit = *stream - '0';
                if (digit > 7) {
                    syntax_error("Invalid digit in octal literal, %c\n", *stream);
                }
                result = result * base + digit;
                stream++;
            }
            token.mod = MOD_OCTAL;
        } else {
            result = 0;
            token.mod = MOD_DECIMAL;
        }
    } else {
        while (isdigit(*stream)) {
            int digit = *stream - '0';
            result = result * base + digit;
            stream++;
        }
        token.mod = MOD_DECIMAL;
    }

    token.type = TOKEN_INTLIT;
    token.int_val = result;
}
void scan_string() {
    stream++;
    char *string = nullptr;
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
                arrpush(string, *stream);
                stream++;
            } else {
                syntax_error("Unsupported escape character, %c\n", *stream);
            }
        } else {
            arrpush(string, *stream);
            stream++;
        }
    }
    arrpush(string, 0);
			
    token.type = TOKEN_STRLIT;
    token.str_val = intern_str(string);
}

void scan_char() {
    stream++;
    char val = *stream;
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
}

void next_token() {
begin:
    token.mod = MOD_NONE;
    const char *start = stream;
	switch (*stream) {
    case ' ': case '\t': case '\r': case '\n':
        stream++;
		goto begin;

    case '<':
        stream++;
        if (*stream == '<') {
            stream++;
            if (*stream == '=') {
                stream++;
                token.type = TOKEN_LSHIFT_ASSIGN;
            } else {
                token.type = TOKEN_LSHIFT;
            }
        } else if (*stream == '=') {
            stream++;
            token.type = TOKEN_LTEQ;
        } else {
            token.type = (Token_Type)'<';
        }
        break;
    case '>':
        stream++;
        if (*stream == '>') {
            stream++;
            if (*stream == '=') {
                stream++;
                token.type = TOKEN_RSHIFT_ASSIGN;
            } else {
                token.type = TOKEN_RSHIFT;
            }
        } else if (*stream == '=') {
            stream++;
            token.type = TOKEN_GTEQ;
        } else {
            token.type = (Token_Type)'>';
        }
        break;

    case '^':
        stream++;
        if (*stream == '=') {
            stream++;
            token.type = TOKEN_XOR_ASSIGN;
        } else {
            token.type = (Token_Type)'^';
        }
        break;
    case '!':
        stream++;
        if (*stream == '=') {
            stream++;
            token.type = TOKEN_NEQ;
        } else {
            token.type = (Token_Type)'!';
        }
        break;
    case '|':
        stream++;
        if (*stream == '=') {
            stream++;
            token.type = TOKEN_OR_ASSIGN;
        } else {
            token.type = (Token_Type)'|';
        }
        break;
    case '&':
        stream++;
        if (*stream == '&') {
            stream++;
            token.type = TOKEN_AND;
        } else if (*stream == '=') {
            stream++;
            token.type = TOKEN_AND_ASSIGN;
        } else {
            token.type = (Token_Type)'&';
        }
        break;
    case '~':
        stream++;
        if (*stream == '=') {
            stream++;
            token.type = TOKEN_NOT_ASSIGN;
        } else {
            token.type = (Token_Type)'~';
        }
        break;

    case '=':
        stream++;
        if (*stream == '=') {
            stream++;
            token.type = TOKEN_EQ;
        } else {
            token.type = (Token_Type)'=';
        }
        break;
    case '+':
        stream++;
        if (*stream == '+') {
            stream++;
            token.type = TOKEN_INC;
        } else if (*stream == '=') {
            stream++;
            token.type = TOKEN_ADD_ASSIGN;
        } else {
            token.type = (Token_Type)'+';
        }
        break;
    case '-':
        stream++;
        if (*stream == '-') {
            stream++;
            token.type = TOKEN_DEC;
        } else if (*stream == '=') {
            stream++;
            token.type = TOKEN_SUB_ASSIGN;
        } else {
            token.type = (Token_Type)'-';
        }
        break;
    case ':':
        stream++;
        if (*stream == '=') {
            stream++;
            token.type = TOKEN_COLON_ASSIGN;
        } else {
            token.type = (Token_Type)':';
        }
        break;
        
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
        scan_char();
    } break;
		
    case '\"': {
        scan_string();
    } break;

    case '/': {
        stream++;
        if (*stream == '/') { // line comment
            stream++;
            while (*stream != 0 && *stream != '\n' && *stream != '\r') {
                stream++;
            }
            goto begin; // whitespace will be skipped next token pass
        } else if (*stream == '*') { // block comment
            stream++;
            while (*stream != 0) {
                if (*stream == '*') {
                    stream++;
                    if (*stream == '/') {
                        stream++;
                        break;
                    }
                } else {
                    stream++;
                }
            }
            goto begin;
        }
    } break;
		
    default:
        token.type = (Token_Type)*stream++;
    }
}

bool is_token(Token_Type type) {
    return token.type == type;
}

bool is_token(char type) {
    return token.type == (Token_Type)type;
}

bool match_token(Token_Type type) {
    if (is_token(type)) {
        next_token();
        return true;
    } else {
        return false;
    }
}

bool match_token(char type) {
    if (is_token(type)) {
        next_token();
        return true;
    } else {
        return false;
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
