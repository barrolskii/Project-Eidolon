#include <stdio.h>
#include "debug.h"

void debug_print_token_header(void)
{
    printf(" %4s | %4s | %20s | %4s | %s\n", "line", "col", "token type", "len", "value");
    printf("-----------------------------------------------------\n");
}

void debug_print_token(token_t tok)
{
    printf(" %4d | %4d | %20s | %4d | %.*s\n",
        tok.line, tok.col, token_get_type_literal(tok.type), tok.len, tok.len, tok.start);
}
