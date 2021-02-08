#include "ast.h"

ast_node_t *init_ast_node(ast_type_t type)
{
    ast_node_t *node = malloc(sizeof(ast_node_t));
    node->next = NULL;
    node->type = type;

    return node;
}

expr_t *init_expr(token_t tok)
{
    expr_t *expr = malloc(sizeof(expr_t));
    expr->left  = NULL;
    expr->right = NULL;
    expr->tok   = tok;

    return expr;
}

void expr_free(expr_t *expr)
{
    if (expr->left) expr_free(expr->left);
    if (expr->right) expr_free(expr->right);
    if (expr) free(expr);
}

void ast_node_free(ast_node_t *node)
{
    if (node->expr) expr_free(node->expr);
    if (node->next) ast_node_free(node->next);
    if (node) free(node);
}

void expr_print_header()
{
    printf(" Type | Left | Right\n");
    printf("-----------------------------------------------------\n");
}

void ast_node_print_header()
{
    printf(" Type\n");
    printf("-----------------------------------------------------\n");
}

void expr_print_expr(expr_t *expr)
{
    printf("%s | %p | %p\n", token_get_type_literal(expr->tok.type), expr->left, expr->right);
    if (expr->left) expr_print_expr(expr->left);
    if (expr->right) expr_print_expr(expr->right);
}

void ast_node_print_node(ast_node_t *node)
{
    printf("%d\n", node->type);
    expr_print_header();
    expr_print_expr(node->expr);

    if (node->next) ast_node_print_node(node->next);
}
