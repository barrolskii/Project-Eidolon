#ifndef AST_H
#define AST_H

#include <stdlib.h>
#include <stdio.h>
#include "lexer.h"

typedef enum {
    AST_EXPR,
    AST_STMT,
    AST_VAR_DECL,

    /* TODO: Make sure these are accurate */
    AST_TERM,
    AST_FACTOR,
    AST_IDENT,
    AST_VAR,
} ast_type_t;

typedef struct expr {
    struct expr *left;
    struct expr *right;
    token_t tok;
} expr_t;

typedef struct ast_node {
    struct ast_node *next;
    ast_type_t type;
    expr_t *expr;
} ast_node_t;

expr_t *init_expr(token_t tok);
ast_node_t *init_ast_node(ast_type_t type);

void expr_free(expr_t *expr);
void ast_node_free(ast_node_t *node) ;

int ast_append_node(ast_node_t *parent, ast_node_t *child);

void expr_print_header();
void ast_node_print_header();

void expr_print_expr(expr_t *expr);
void ast_node_print_node(ast_node_t *node);

#endif //AST_H

