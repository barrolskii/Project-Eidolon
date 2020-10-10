#ifndef __AST_H_
#define __AST_H_

#include "token.h"

typedef enum {
    EXPRESSION = 0,
    STATEMENT
} node_type_t;

typedef struct {
    token_type_t type;
    node_type_t node_type;
    char *literal;
} statement_t, expression_t;

struct node {
    statement_t *data;
    struct node *next;
};

typedef struct program {
    struct node *head;
    unsigned count;
} program_t;

program_t *ast_init_program();
int ast_append_node(program_t *prog, statement_t *stmt);

char *ast_get_literal();
token_type_t ast_get_type();

#endif // __AST_H_
