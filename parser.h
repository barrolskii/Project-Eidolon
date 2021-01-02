#ifndef __PHANTOM_PARSER_H_
#define __PHANTOM_PARSER_H_

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "lexer.h"
#include "vm.h"

typedef struct {
    token_t prev;
    token_t curr;
    bool had_err;
    lexer_t *l;
    vm_t *vm;
} parser_t;

parser_t *parser_init(const char *src, vm_t *vm);
void parser_free(parser_t *p);

void parser_advance(parser_t *p);
void parser_parse(parser_t *p);

#endif // __PHANTOM_PARSER_H_
