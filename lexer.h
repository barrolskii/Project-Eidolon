#ifndef __PHANTOM_LEXER_H_
#define __PHANTOM_LEXER_H_

typedef enum {
    TOK_ILLEGAL,
    TOK_ERROR,
    TOK_EOF,

    /* Identifiers, assignment, and types */
    TOK_IDENT,
    TOK_ASSIGN,
    TOK_INT,
    TOK_FLOAT,
    TOK_STRING,

    /* Arithmetic operators */
    TOK_PLUS,
    TOK_MINUS,
    TOK_DIVIDE,
    TOK_MULTIPLY,
    TOK_MODULO,

    /* Logical operators */
    TOK_BANG,
    TOK_AND,
    TOK_OR,
    TOK_INCREMENT,
    TOK_DECREMENT,

    /* Comparison operators */
    TOK_LT,
    TOK_GT,
    TOK_NE,
    TOK_EQ,
    TOK_LT_EQ,
    TOK_GT_EQ,

    TOK_COMMA,
    TOK_SEMICOLON,
    TOK_COMMENT,

    TOK_LPAREN,
    TOK_RPAREN,
    TOK_LBRACE,
    TOK_RBRACE,
    TOK_LBRACKET,
    TOK_RBRACKET,

    /* Keywords */
    TOK_VAR,
    TOK_IF,
    TOK_ELSE,
    TOK_LOOP,
    TOK_FUNC,
    TOK_RETURN,
    TOK_BREAK,
    TOK_CONTINUE,
    TOK_TRUE,
    TOK_FALSE,

    TOK_STDIN,
} token_type;

typedef struct {
    token_type type;
    const char *start; /* Start of the token in the source code */
    unsigned len;      /* Length of the token in the input */
    unsigned line;     /* Line in the source code */
    unsigned col;      /* Current column in the current line */
} token_t;

typedef struct {
    const char *start;
    const char *curr;
    unsigned line;
    unsigned col;
} lexer_t;

lexer_t *lexer_init(const char *src);
void lexer_free(lexer_t *l);
token_t lexer_next(lexer_t *l);
const char *token_get_type_literal(token_type type);

#endif // __PHANTOM_LEXER_H_
