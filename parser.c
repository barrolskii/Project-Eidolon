#include "parser.h"

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

typedef void (*parse_func)(parser_t *p);

typedef struct {
    parse_func prefix;
    parse_func infix;
    op_prec prec;
} parse_rule_t;

/* Function declarations */
void parser_advance(parser_t *p, lexer_t *l);
static void number_int(parser_t *p);

parse_rule_t parse_rules[] = {
    /*              prefix  infix  operator precedence */

    [TOK_ILLEGAL] = { NULL, NULL, OP_PREC_NONE },
    [TOK_ERROR]   = { NULL, NULL, OP_PREC_NONE },
    [TOK_EOF]     = { NULL, NULL, OP_PREC_NONE },

    [TOK_IDENT]  = { NULL, NULL, OP_PREC_NONE },
    [TOK_ASSIGN] = { NULL, NULL, OP_PREC_ASSIGN },
    [TOK_INT]    = { number_int, NULL, OP_PREC_NONE },
    [TOK_FLOAT]  = { NULL, NULL, OP_PREC_NONE },
    [TOK_STRING] = { NULL, NULL, OP_PREC_NONE },

    [TOK_PLUS]     = { NULL, NULL, OP_PREC_TERM },
    [TOK_MINUS]    = { NULL, NULL, OP_PREC_TERM },
    [TOK_DIVIDE]   = { NULL, NULL, OP_PREC_FACTOR },
    [TOK_MULTIPLY] = { NULL, NULL, OP_PREC_FACTOR },
    [TOK_MODULO]   = { NULL, NULL, OP_PREC_FACTOR },

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

static bool match_tok(token_t tok, token_type type)
{
    return tok.type == type;
}

static void parser_err(parser_t *p, const char *err_msg)
{
     fprintf(stderr, "[line %d : col %d] Error: %s\n", p->curr.line, p->curr.col, err_msg);
}

static void parser_err_at_curr(parser_t *p)
{
    fprintf(stderr, "[line %d : col %d] Error: ", p->curr.line, p->curr.col);

    if (p->curr.type == TOK_ILLEGAL)
        fprintf(stderr, "illegal character %.*s\n", p->curr.len, p->curr.start);
}

static void consume_tok(parser_t *p, lexer_t *l, token_type type, const char *err_msg)
{
    if (p->curr.type == type)
    {
        parser_advance(p, l);
        return;
    }

    parser_err(p, err_msg);
}

parser_t *parser_init()
{
    parser_t *p = malloc(sizeof(parser_t));
    p->had_err = false;

    return p;
}

void parser_free(parser_t *p)
{
    free(p);
}

void parser_advance(parser_t *p, lexer_t *l)
{
    p->prev = p->curr;

    p->curr = lexer_next(l);

    if (p->curr.type == TOK_ERROR || p->curr.type == TOK_ILLEGAL)
        parser_err_at_curr(p);
}

static void number_int(parser_t *p)
{
    long value = strtol(p->curr.start, NULL, 10);
    // TODO: print value for now
    printf("value: %lu\n", value);
}

static parse_rule_t *get_rule(token_type type)
{
    return &parse_rules[type];
}

static void parse_precedence(parser_t *p, lexer_t *l, op_prec prec)
{
    parse_func prefix_rule = get_rule(p->curr.type)->prefix;
    if (!prefix_rule)
    {
        parser_err(p, "Expected expression");
        return;
    }

    bool can_assign = prec <= OP_PREC_ASSIGN;
    prefix_rule(p);

    parser_advance(p, l);
}

static void temp_expr()
{

}

static uint8_t temp_parse_var(parser_t *p, lexer_t *l)
{
    parser_advance(p, l);
    consume_tok(p, l, TOK_IDENT, "Expected variable name after var");

    return 0;
}

static void temp_var_declaration(parser_t *p, lexer_t *l)
{
    // parse the variable and then assign it to a variable
    // match the equals token
    // consume the semicolon
    // define the variable

    uint8_t global = temp_parse_var(p, l);

    //if (match_tok(p->curr, TOK_ASSIGN))

    // Error for now but default the value to NULL or nil
    // whatever one you find is better
    consume_tok(p, l, TOK_ASSIGN, "Expected '=' after variable name");
    parse_precedence(p, l, OP_PREC_ASSIGN);

    consume_tok(p, l, TOK_SEMICOLON, "Expected ';' after variable declaration");

    // TODO: Define variable.
}

static void temp_declaration(parser_t *p, lexer_t *l)
{
    switch (p->curr.type)
    {
        case TOK_FUNC:
            // func declaration
        case TOK_VAR:
            temp_var_declaration(p, l);
        default:
            // statement
            return;
    }
}

void parser_parse(parser_t *p, lexer_t *l)
{
    parser_advance(p, l);

    while (!match_tok(p->curr, TOK_EOF))
    {
        temp_declaration(p, l);

        parser_advance(p, l);
    }
}
