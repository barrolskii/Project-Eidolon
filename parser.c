#include "parser.h"
#include "debug.h"
#include "lexer.h"

typedef enum {
    OP_PREC_NONE,
    OP_PREC_ASSIGN,             /* = */
    OP_PREC_OR,                 /* || */
    OP_PREC_AND,                /* && */
    OP_PREC_EQUALITY,           /* == != */
    OP_PREC_COMPARISON,         /* < > <= >= */
    OP_PREC_TERM,               /* + - */
    OP_PREC_FACTOR,             /* / * % */
    OP_PREC_UNARY,              /*  ! - */
    OP_PREC_CALL,               /* . () */
    OP_PREC_PRIMARY
} op_prec; /* Operator precedence */

typedef expr_t *(*parse_func)(parser_t *p);

typedef struct {
    parse_func prefix;
    parse_func infix;
    op_prec prec;
} parse_rule_t;

static void parser_advance(parser_t *p)
{
    p->prev = p->curr;
    p->curr = lexer_next(p->l);
}

static void parser_err(parser_t *p, char *err_msg)
{
    fprintf(stderr, "[line %d: col: %d] Error: %s\n", p->curr.line, p->curr.col, err_msg);
}

static void consume_tok(parser_t *p, token_type type, char *err_msg)
{
    if (p->curr.type == type)
    {
        parser_advance(p);
        return;
    }

    parser_err(p, err_msg);
}

static expr_t *variable(parser_t *p)
{
    printf("variable\n");

    return NULL;
}

static expr_t *number_int(parser_t *p)
{
    printf("number_int\n");
    //printf("%s -- %.*s\n", token_get_type_literal(p->prev.type), p->prev.len, p->prev.start);

    expr_t *num = init_expr(p->prev);

    return num;
}

static expr_t *number_double(parser_t *p)
{
    printf("number_double\n");

    return NULL;
}

static expr_t *string(parser_t *p)
{
    printf("string\n");

    return NULL;
}

static expr_t *binary_op(parser_t *p)
{
    printf("binary_op\n");

    expr_t *op = init_expr(p->prev);
    op->right = init_expr(p->curr);

    parser_advance(p);

    return op;
}

static parse_rule_t parse_rules[] = {
    /*              prefix  infix  operator precedence */

    [TOK_ILLEGAL] = { NULL, NULL, OP_PREC_NONE },
    [TOK_ERROR]   = { NULL, NULL, OP_PREC_NONE },
    [TOK_EOF]     = { NULL, NULL, OP_PREC_NONE },

    [TOK_IDENT]  = { variable,       NULL, OP_PREC_NONE },
    [TOK_ASSIGN] = { NULL,       NULL, OP_PREC_ASSIGN },
    [TOK_INT]    = { number_int, NULL, OP_PREC_NONE },
    [TOK_FLOAT]  = { number_double, NULL, OP_PREC_NONE },
    [TOK_STRING] = { string,       NULL, OP_PREC_NONE },

    [TOK_PLUS]     = { NULL, binary_op, OP_PREC_TERM },
    [TOK_MINUS]    = { NULL, binary_op, OP_PREC_TERM },
    [TOK_DIVIDE]   = { NULL, binary_op, OP_PREC_FACTOR },
    [TOK_MULTIPLY] = { NULL, binary_op, OP_PREC_FACTOR },
    [TOK_MODULO]   = { NULL, binary_op, OP_PREC_FACTOR },

    [TOK_BANG]      = { NULL, NULL, OP_PREC_UNARY },
    [TOK_AND]       = { NULL, NULL, OP_PREC_AND },
    [TOK_OR]        = { NULL, NULL, OP_PREC_OR },
    [TOK_INCREMENT] = { NULL, NULL, OP_PREC_UNARY },
    [TOK_DECREMENT] = { NULL, NULL, OP_PREC_UNARY },

    [TOK_LT]    = { NULL, NULL, OP_PREC_EQUALITY },
    [TOK_GT]    = { NULL, NULL, OP_PREC_EQUALITY },
    [TOK_NE]    = { NULL, NULL, OP_PREC_COMPARISON },
    [TOK_EQ]    = { NULL, NULL, OP_PREC_COMPARISON },
    [TOK_LT_EQ] = { NULL, NULL, OP_PREC_EQUALITY },
    [TOK_GT_EQ] = { NULL, NULL, OP_PREC_EQUALITY },

    [TOK_COMMA]     = { NULL, NULL, OP_PREC_NONE },
    [TOK_SEMICOLON] = { NULL, NULL, OP_PREC_NONE },
    [TOK_COMMENT]   = { NULL, NULL, OP_PREC_NONE },

    [TOK_LPAREN]   = { NULL, NULL, OP_PREC_CALL },
    [TOK_RPAREN]   = { NULL, NULL, OP_PREC_NONE },
    [TOK_LBRACE]   = { NULL, NULL, OP_PREC_NONE },
    [TOK_RBRACE]   = { NULL, NULL, OP_PREC_NONE },
    [TOK_LBRACKET] = { NULL, NULL, OP_PREC_NONE },
    [TOK_RBRACKET] = { NULL, NULL, OP_PREC_NONE },

    [TOK_VAR]      = { NULL, NULL, OP_PREC_NONE },
    [TOK_IF]       = { NULL, NULL, OP_PREC_NONE },
    [TOK_ELSE]     = { NULL, NULL, OP_PREC_NONE },
    [TOK_LOOP]     = { NULL, NULL, OP_PREC_NONE },
    [TOK_FUNC]     = { NULL, NULL, OP_PREC_NONE },
    [TOK_RETURN]   = { NULL, NULL, OP_PREC_NONE },
    [TOK_BREAK]    = { NULL, NULL, OP_PREC_NONE },
    [TOK_CONTINUE] = { NULL, NULL, OP_PREC_NONE },
    [TOK_TRUE]     = { NULL, NULL, OP_PREC_NONE },
    [TOK_FALSE]    = { NULL, NULL, OP_PREC_NONE },
};

static parse_rule_t *get_rule(token_type type)
{
    return &parse_rules[type];
}

static expr_t *parse_precedence(parser_t *p, op_prec prec)
{
    parser_advance(p);

    parse_func prefix_rule = get_rule(p->prev.type)->prefix;
    if (!prefix_rule)
    {
        parser_err(p, "Expected expression");
        return NULL; /* TODO: Return null for now. Need to return error nodes */
    }

    /* TODO: Remove comment. This should contain the binary expression */
    expr_t *prefix = prefix_rule(p);
    expr_t *infix = NULL;

    while (prec <= get_rule(p->curr.type)->prec)
    {
        printf("In while loop\n");
        parser_advance(p);
        parse_func infix_rule = get_rule(p->prev.type)->infix;
        infix = infix_rule(p);

        /* Set the left node of the expression to be the prefix of the expression */
        infix->left = prefix;
    }

    return infix;
}

static ast_node_t *expression(parser_t *p)
{
    printf("In expression\n");

    ast_node_t *ast_node = init_ast_node(AST_EXPR);
    expr_t *expr = parse_precedence(p, OP_PREC_ASSIGN);

    ast_node->expr = expr;

    consume_tok(p, TOK_SEMICOLON, "Expected ';' at the end of expression");

    return ast_node;
}

static ast_node_t *statement(parser_t *p)
{
    switch (p->curr.type)
    {
        case TOK_IF:
        case TOK_RETURN:
        case TOK_LOOP:
        case TOK_LBRACE:
        {
            printf("Statements not yet implemented\n");
            return NULL; /* TODO: Error node here */
        }
        default:
            return expression(p);
    }
}

static int match_tok(token_t tok, token_type type)
{
    return tok.type == type;
}

static ast_node_t *var_decl(parser_t *p)
{
    /* TODO: Add expression recursive decent parsing here to allow for expressions to be */
    /* parsed correctly */

    printf("Var decl start\n");

    /* Skip the current var token and advance to the ident token */
    parser_advance(p);

    consume_tok(p, TOK_IDENT, "Expected variable definition");

    ast_node_t *var_node = init_ast_node(AST_VAR_DECL);
    printf("Ident: %.*s\n", p->prev.len, p->prev.start);

    expr_t *expr = init_expr(p->curr); /* Assignment token '=' */
    expr->left = init_expr(p->prev); /* The ident token */

    parser_advance(p);
    expr->right = init_expr(p->curr); /* The value of the assignment */
    parser_advance(p);

    var_node->expr = expr;

    consume_tok(p, TOK_SEMICOLON, "Expected ';' at the end of expression");

    printf("Var decl end\n");

    return var_node;
}

parser_t *parser_init(lexer_t *l)
{
    parser_t *p = malloc(sizeof(parser_t));
    p->l = l;
    p->prev = lexer_next(l);
    p->curr = p->prev;

    return p;
}

void parser_free(parser_t *p)
{
    lexer_free(p->l);
    free(p);
}

ast_node_t *parser_parse_program(parser_t *p)
{
    ast_node_t *ast = NULL;    /* The ast that the parser will return */
    ast_node_t *curr = NULL;   /* Pointer to keep track of where we are in the tree */

    while (!match_tok(p->curr, TOK_EOF))
    {
        switch (p->curr.type)
        {
            case TOK_FUNC:
            case TOK_VAR:
            {
                ast_node_t *node = var_decl(p);

                if (!ast)
                {
                    ast = node;
                    curr = ast;
                    break;
                }

                curr->next = node;
                curr = curr->next;

                break;
            }
            default:
            {
                ast_node_t *expr_node = statement(p);

                if (!ast)
                {
                    ast = expr_node;
                    curr = ast;
                    break;
                }

                curr->next = expr_node;
                curr = curr->next;
            }
        }
    }

    return ast;
}
