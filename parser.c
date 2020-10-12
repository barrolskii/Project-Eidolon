#include "parser.h"

#include <stdio.h>

parser_t *parser_init(lexer_t *l)
{
    parser_t *p = malloc(sizeof(parser_t));
    p->l = l;
    p->cur_tok = NULL;
    p->peek_tok = NULL;
    p->errors = NULL;
    p->err_count = 0;

    /* Read two tokens so both tokens are set */
    parser_next_token(p);
    parser_next_token(p);

    return p;
}

void parser_next_token(parser_t *p)
{
    p->cur_tok = p->peek_tok;
    p->peek_tok = next_token(p->l);
}

int parser_curr_token_is(parser_t *p, token_type_t t)
{
    return p->cur_tok->type == t;
}

int parser_peek_token_is(parser_t *p, token_type_t t)
{
    return p->peek_tok->type == t;
}

int parser_expected_peek(parser_t *p, token_type_t t)
{
    if (parser_peek_token_is(p, t))
    {
        parser_next_token(p);
        return 1;
    }
    else
    {
        parser_peek_error(p, t);
        return 0;
    }
}

statement_t *parser_parse_var_statement(parser_t *p)
{
    statement_t *stmt = malloc(sizeof(statement_t));
    stmt->type = p->cur_tok->type;

    if (!parser_expected_peek(p, IDENT))
    {
        free(stmt);
        return NULL;
    }

    stmt->node_type = EXPRESSION;
    stmt->literal = malloc(sizeof(char) * (strlen(p->cur_tok->literal) + 1));
    strcpy(stmt->literal, p->cur_tok->literal);

    if (!parser_expected_peek(p, ASSIGN))
    {
        free(stmt);
        return NULL;
    }

    /* TODO: We're skipping expressions until we
       encounter a semicolon */

    while (!parser_curr_token_is(p, SEMI_COLON))
    {
        parser_next_token(p);
    }

    return stmt;
}

statement_t *parser_parse_return_statement(parser_t *p)
{
    statement_t *stmt = malloc(sizeof(statement_t));
    stmt->type = p->cur_tok->type;

    parser_next_token(p);

    stmt->node_type = STATEMENT;
    stmt->literal = malloc(sizeof(char) * (strlen(p->cur_tok->literal) + 1));
    strcpy(stmt->literal, p->cur_tok->literal);


    /* TODO: We're skipping the expression until we encounter a semicolon */
    while (p->cur_tok->type != SEMI_COLON)
    {
        parser_next_token(p);
    }

    return stmt;
}

statement_t *parser_parse_statement(parser_t *p)
{
    switch (p->cur_tok->type)
    {
        case VAR:
            return parser_parse_var_statement(p);
        case RETURN:
            return parser_parse_return_statement(p);
        default:
            return NULL;
    }
}

program_t *parser_parse_program(parser_t *p)
{
    program_t *prog = ast_init_program();
    statement_t *stmt = NULL;

    while(p->cur_tok->type != END_OF_FILE)
    {
        stmt = parser_parse_statement(p);
        if (stmt) { ast_append_node(prog, stmt); }

        parser_next_token(p);
    }

    return prog;
}

void parser_append_error(parser_t *p, char *err_msg)
{
    if (!p->errors)
    {
        p->errors = malloc(sizeof(struct error));
        p->errors->next = NULL;
        p->errors->error = err_msg;
        p->err_count++;

        return;
    }

    /* Point to the head of the error list */
    struct error *curr_err = p->errors;

    while (curr_err->next)
    {
        curr_err = curr_err->next;
    }

    curr_err->next = malloc(sizeof(struct error));
    curr_err->next->next = NULL;
    curr_err->next->error = err_msg;
    p->err_count++;
}

void parser_peek_error(parser_t *p, token_type_t t)
{
    char *msg = calloc(MAX_ERR_LEN, sizeof(char));
    sprintf(msg, "expected next token to be %s, got %s", get_type_literal(t), get_type_literal(p->peek_tok->type));
    parser_append_error(p, msg);
}
