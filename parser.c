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
static void parse_precedence(parser_t *p, op_prec prec);
void parser_advance(parser_t *p);
static parse_rule_t *get_rule(token_type type);
static void binary_op(parser_t *p);
static void number_int(parser_t *p);

parse_rule_t parse_rules[] = {
    /*              prefix  infix  operator precedence */

    [TOK_ILLEGAL] = { NULL, NULL, OP_PREC_NONE },
    [TOK_ERROR]   = { NULL, NULL, OP_PREC_NONE },
    [TOK_EOF]     = { NULL, NULL, OP_PREC_NONE },

    [TOK_IDENT]  = { NULL,       NULL, OP_PREC_NONE },
    [TOK_ASSIGN] = { NULL,       NULL, OP_PREC_ASSIGN },
    [TOK_INT]    = { number_int, NULL, OP_PREC_NONE },
    [TOK_FLOAT]  = { NULL,       NULL, OP_PREC_NONE },
    [TOK_STRING] = { NULL,       NULL, OP_PREC_NONE },

    [TOK_PLUS]     = { NULL, binary_op, OP_PREC_TERM },
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

static void consume_tok(parser_t *p, token_type type, const char *err_msg)
{
    if (p->curr.type == type)
    {
        parser_advance(p);
        return;
    }

    parser_err(p, err_msg);
}

parser_t *parser_init(const char *src, vm_t *vm)
{
    parser_t *p = malloc(sizeof(parser_t));
    p->had_err = false;
    p->l = lexer_init(src);
    p->vm = vm;

    return p;
}

void parser_free(parser_t *p)
{
    lexer_free(p->l);
    free(p);
}

void parser_advance(parser_t *p)
{
    p->prev = p->curr;

    p->curr = lexer_next(p->l);

    if (p->curr.type == TOK_ERROR || p->curr.type == TOK_ILLEGAL)
        parser_err_at_curr(p);
}

static void emit_byte(parser_t *p, uint8_t op)
{
    uint8_t ip = p->vm->ip;
    p->vm->instructions[ip] = op;
    p->vm->ip++;
}

static void binary_op(parser_t *p)
{
    /* Keep track of the current operator */
    token_type operator_type = p->prev.type;

    /* Push the right operand onto the stack before the operator */
    parse_rule_t *rule = get_rule(operator_type);
    parse_precedence(p, rule->prec + 1);

    printf("Binary operation\n");

    /* TODO: Add other binary operators here */
    switch (operator_type)
    {
        case TOK_PLUS: emit_byte(p, OP_ADD); break;
        default:
           printf("Other binary operators not yet implemented\n");
    }
}

static void number_int(parser_t *p)
{
    /* TODO: Update this to use the garbage collector */

    long value = strtol(p->prev.start, NULL, 10);
    object_t obj = { .type = OBJ_VAL_LONG, .as.long_num = value };

    uint8_t sp = p->vm->sp;

    /* Add the value to the object stack and increment the stack pointer */
    p->vm->stack[sp] = obj;
    p->vm->sp++;

    emit_byte(p, OP_CONST);
}

static parse_rule_t *get_rule(token_type type)
{
    return &parse_rules[type];
}

static void parse_precedence(parser_t *p, op_prec prec)
{
    parser_advance(p);

    parse_func prefix_rule = get_rule(p->prev.type)->prefix;
    if (!prefix_rule)
    {
        parser_err(p, "Expected expression");
        return;
    }

    prefix_rule(p);

    while (prec <= get_rule(p->curr.type)->prec)
    {
        parser_advance(p);
        parse_func infix_rule = get_rule(p->prev.type)->infix;
        infix_rule(p);
    }
}

static void expression(parser_t *p)
{
    parse_precedence(p, OP_PREC_ASSIGN);
    consume_tok(p, TOK_SEMICOLON, "Expected ';' at the end of expression");

    /* TODO: Check if this is needed or not */
    //emit_byte(p, OP_POP);
}

static void statement(parser_t *p)
{
    switch(p->curr.type)
    {
        case TOK_IF:
        case TOK_RETURN:
        case TOK_LOOP:
        case TOK_LBRACE:
        {
            printf("Statements not yet implemented\n");
            return;
        }
        default:
            expression(p);
    }
}

void parser_parse(parser_t *p)
{
    parser_advance(p);

    while (!match_tok(p->curr, TOK_EOF))
    {
        /* Check the declaration type */
        switch (p->curr.type)
        {
            case TOK_FUNC:
            case TOK_VAR:
            {
                printf("Var and func declarations not yet implemented\n");
                return;
            }
            default:
                statement(p);
        }

        parser_advance(p);
    }
}