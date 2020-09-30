#ifndef __LEXER_H_
#define __LEXER_H_

#include <string.h>
#include <stdlib.h>

#include "token.h"


typedef struct lexer {
    char *input;
    int pos;      /* Current position in input */
    int read_pos; /* Current reading position */
    char ch;
} lexer_t;


/* Lexer utility functions */
int is_letter(char c);
int is_digit(char c);
int str_cont(char c, char *str);


lexer_t *lexer_init(char *input);

void lexer_read_char(lexer_t *l);
char lexer_peek_char(lexer_t *l);

char *lexer_read_digit(lexer_t *l);
char *lexer_read_identifier(lexer_t *l);
char *lexer_read_letter(lexer_t *l);
char *lexer_read_string(lexer_t *l);

void lexer_skip_whitespace(lexer_t *l);


struct token *next_token(lexer_t *l);

#endif // __LEXER_H_
