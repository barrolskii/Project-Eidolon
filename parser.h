#ifndef __PARSER_H_
#define __PARSER_H_

#include "token.h"
#include "lexer.h"
#include "ast.h"

#define MAX_ERR_LEN 1024

struct error {
    struct error *next;
    char *error;
};

typedef struct parser {
    lexer_t *l;

    struct token *cur_tok;
    struct token *peek_tok;
    struct error *errors;
    unsigned err_count;
} parser_t;

parser_t *parser_init(lexer_t *l);

void parser_next_token(parser_t *p);
int parser_curr_token_is(parser_t *p, token_type_t t);
int parser_peek_token_is(parser_t *p, token_type_t t);
int parser_expected_peek(parser_t *p, token_type_t t);

statement_t *parser_parse_var_statement(parser_t *p);
statement_t *parser_parse_statement(parser_t *p);
program_t *parser_parse_program(parser_t *p);

void parser_append_error(parser_t *p, char *err_msg);
void parser_peek_error(parser_t *p, token_type_t t);

#endif // __PARSER_H_
