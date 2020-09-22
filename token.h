#include <string.h> // For strcmp

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

char *type_literals[] = {
    "ILLEGAL",
    "END_OF_FILE",

    "IDENT",
    "INT",
    "FLOAT",
    "CHAR",
    "STRING",

    "ASSIGN",
    "PLUS",
    "MINUS",
    "DIVIDE",
    "MULTIPLY",
    "MODULO",
    "BANG",

    "AND",
    "OR",
    "INCREMENT",
    "DECREMENT",

    "COMMA",
    "SEMI_COLON",

    "LPAREN",
    "RPAREN",
    "LBRACE",
    "RBRACE",
    "LBRACKET",
    "RBRACKET",

    "LT",
    "GT",
    "EQ",
    "NE",
    "LT_EQ",
    "GT_EQ",

    "VAR",
    "IF",
    "ELSE",
    "LOOP",
    "FUNC",
    "RETURN",
    "BREAK",
    "CONTINUE",
    "TRUE",
    "FALSE",

    "ARRAY",
    "HASHTABLE",
};

struct token {
    token_type_t type;
    char* literal;
};

char *get_type_literal(token_type_t t)
{
    return type_literals[t];
}

token_type_t get_identifier(const char *i)
{
    if (strcmp(i, "var") == 0) return VAR;
    if (strcmp(i, "if") == 0) return IF;
    if (strcmp(i, "else") == 0) return ELSE;
    if (strcmp(i, "loop") == 0) return LOOP;
    if (strcmp(i, "func") == 0) return FUNC;
    if (strcmp(i, "return") == 0) return RETURN;
    if (strcmp(i, "break") == 0) return BREAK;
    if (strcmp(i, "continue") == 0) return CONTINUE;
    if (strcmp(i, "true") == 0) return TRUE;
    if (strcmp(i, "false") == 0) return FALSE;

    return IDENT;
}
