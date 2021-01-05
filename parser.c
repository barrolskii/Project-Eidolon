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
    /* Get the index of the byte instruction */
    uint8_t index = p->vm->instruct_count;

    p->vm->instructions[index] = op;
    p->vm->instruct_count++;
}

static void binary_op(parser_t *p)
{
    /* Keep track of the current operator */
    token_type operator_type = p->prev.type;

    /* Push the right operand onto the stack before the operator */
    parse_rule_t *rule = get_rule(operator_type);
    parse_precedence(p, rule->prec + 1);

    switch (operator_type)
    {
        case TOK_PLUS: emit_byte(p, OP_ADD); break;
        case TOK_MINUS: emit_byte(p, OP_SUB); break;
        case TOK_MULTIPLY: emit_byte(p, OP_MUL); break;
        case TOK_DIVIDE: emit_byte(p, OP_DIV); break;
        case TOK_MODULO: emit_byte(p, OP_MOD); break;
        default: break;
            /* Unreachable */
    }
}

static void number_int(parser_t *p)
{
    /* TODO: Update this to use the garbage collector */

    long value = strtol(p->prev.start, NULL, 10);
    object_t obj = { .type = OBJ_VAL_LONG, .as.long_num = value };

    uint8_t const_count = p->vm->const_count;

    /* Add the value to the constant list */
    p->vm->constants[const_count] = obj;
    p->vm->const_count++;

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
}

static void expression_statement(parser_t *p)
{
    expression(p);
    consume_tok(p, TOK_SEMICOLON, "Expected ';' at the end of expression");
    emit_byte(p, OP_POP);
}

static void var_declaration(parser_t *p)
{
    /* Skip the var token to the next identifier token */
    parser_advance(p);

    printf("Inside var declaration\n");

    consume_tok(p, TOK_IDENT, "Expected variable identifier");

    char *ident = malloc(sizeof(char) * p->curr.len);            // TODO: memory leak! get garbage collector asap
    memccpy(ident, p->prev.start, *p->prev.start, p->prev.len); // TODO: change this from c11 func to c99

    // Create an object for the name and put it on the stack
    object_t obj = { .type = OBJ_VAL_STR, .as.str = ident };
    printf("obj str: %s\n", obj.as.str);

    // Add the string to the constants list
    uint8_t const_count = p->vm->const_count;
    p->vm->constants[const_count] = obj;
    p->vm->const_count++;

    // Emit the instruction
    emit_byte(p, OP_CONST);

    parser_advance(p);

    expression(p);

    emit_byte(p, OP_ADD_GLOBAL);
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
            expression_statement(p);
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
                printf("Starting var declaration\n");
                var_declaration(p);
                return;
            }
            default:
                statement(p);
        }

        parser_advance(p);
    }
}
