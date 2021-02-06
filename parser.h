#ifndef PARSER_H
#define PARSER_H

#include <stdlib.h>
#include <stdio.h>

#include "lexer.h"
#include "ast.h"

typedef struct {
	token_t prev;
	token_t curr;
	lexer_t *l;
} parser_t;

parser_t *parser_init(lexer_t *l);
void parser_free(parser_t *p);
ast_node_t *parser_parse_program(parser_t *p);

#endif //PARSER_H

