#include "lexer.h"
#include "tests.h"

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

int main(int argc, char **argv) {
    init_lexer();
    lexer_tests();
    return 0;
}
