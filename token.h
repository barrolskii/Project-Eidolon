#ifndef __TOKEN_H_
#define __TOKEN_H_

#include <string.h> // For strcmp
#include <stdlib.h>

/* Token definitions */
typedef enum {
    ILLEGAL = 0,
    END_OF_FILE,

    /* Identifiers and types */
    IDENT,
    INT,
    FLOAT,
    CHAR,
    STRING,

    /* Arithmetic operators */
    ASSIGN,
    PLUS,
    MINUS,
    DIVIDE,
    MULTIPLY,
    MODULO,
    BANG,

    AND,
    OR,
    INCREMENT,
    DECREMENT,

    COMMA,
    SEMI_COLON,

    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    LBRACKET,
    RBRACKET,

    /* Comparison operators */
    LT,
    GT,
    EQ,
    NE,
    LT_EQ,
    GT_EQ,

    /* Keywords */
    VAR,
    IF,
    ELSE,
    LOOP,
    FUNC,
    RETURN,
    BREAK,
    CONTINUE,
    TRUE,
    FALSE,

    ARRAY,
    HASHTABLE,
} token_type_t;

struct token {
    token_type_t type;
    char *literal;
};

struct token *new_token(token_type_t t, char *literal);
char *get_type_literal(token_type_t t);
token_type_t get_identifier(const char *i);

#endif // __TOKEN_H_
