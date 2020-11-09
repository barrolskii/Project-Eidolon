#ifndef __PHANTOM_PARSER_H_
#define __PHANTOM_PARSER_H_

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "lexer.h"

typedef struct {
    token_t prev;
    token_t curr;
    bool had_err;
} parser_t;

parser_t *parser_init();
void parser_free(parser_t *p);

void parser_advance(parser_t *p, lexer_t *l);
void parser_parse(parser_t *p, lexer_t *l);

#endif // __PHANTOM_PARSER_H_
