#ifndef __PARSER_H_
#define __PARSER_H_

#include "token.h"
#include "lexer.h"
#include "ast.h"

typedef struct parser {
    lexer_t *l;

    struct token *cur_tok;
    struct token *peek_tok;
} parser_t;

parser_t *parser_init(lexer_t *l);

void parser_next_token(parser_t *p);
int parser_curr_token_is(parser_t *p, token_type_t t);
int parser_peek_token_is(parser_t *p, token_type_t t);
int parser_expected_peek(parser_t *p, token_type_t t);

statement_t *parser_parse_var_statement(parser_t *p);
statement_t *parser_parse_statement(parser_t *p);
program_t *parser_parse_program(parser_t *p);

#endif // __PARSER_H_
