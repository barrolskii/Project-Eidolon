#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"

static int is_digit(char c)
{
    return c >= '0' && c <= '9';
}

static int is_char(char c)
{
    return (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c == '_');
}

static int is_script_end(char c)
{
    return c == '\0';
}

static char advance(lexer_t *l)
{
    l->curr++;
    return l->curr[-1];
}

static char peek_char(lexer_t *l)
{
    return *l->curr;
}

static char peek_next_char(lexer_t *l)
{
    if (is_script_end(*l->curr)) return '\0';

    return l->curr[1];
}

static int match_char(lexer_t *l, char c)
{
    if (is_script_end(*l->curr)) return 0;
    if (*l->curr != c) return 0;

    l->curr++;
    l->col++;

    return 1;
}

static void skip_whitespace(lexer_t *l)
{
    char c;

    for (;;)
    {
        c = peek_char(l);
        switch(c)
        {
            case ' ':
            case '\r':
            case '\t':
                advance(l);
                break;
            case '\n':
                l->line++;
                advance(l);
                break;
            case '#':
                while( (c = peek_char(l)) != '\n' && !is_script_end(c)) advance(l);
                break;
            default:
                return;
        }
    }
}

static token_t new_token(lexer_t *l, token_type type)
{
    token_t tok;
    tok.type = type;
    tok.start = l->start;
    tok.len = (unsigned)(l->curr - l->start);
    tok.line = l->line;
    tok.col = l->col;

    return tok;
}

static token_t new_identifer(lexer_t *l)
{
    while (is_char(peek_char(l)) || is_digit(peek_char(l))) advance(l);

    return new_token(l, TOK_IDENT);
}

static token_t new_number(lexer_t *l)
{
    token_type type = TOK_INT;

    while (is_digit(peek_char(l)))
    {
        //if (peek_char(l) == '.') type = TOK_FLOAT;
        advance(l);
    }

    return new_token(l, type);
}

static token_t next_token(lexer_t *l)
{
    skip_whitespace(l);

    l->start = l->curr;

    if (is_script_end(*l->curr)) return new_token(l, TOK_EOF);

    char c = advance(l);

    if (is_char(*l->curr)) return new_identifer(l);
    if (is_digit(*l->curr)) return new_number(l);

    switch(c)
    {
        case '=': return new_token(l, match_char(l, '=') ? TOK_EQ : TOK_ASSIGN);

        case '+': return new_token(l, match_char(l, '+') ? TOK_INCREMENT : TOK_PLUS);
        case '-': return new_token(l, match_char(l, '-') ? TOK_DECREMENT : TOK_MINUS);
        case '/': return new_token(l, TOK_DIVIDE);
        case '*': return new_token(l, TOK_MULTIPLY);
        case '%': return new_token(l, TOK_MODULO);

        case '!': return new_token(l, match_char(l, '=') ? TOK_NE : TOK_BANG);
        case '&': return new_token(l, match_char(l, '&') ? TOK_AND : TOK_ILLEGAL);
        case '|': return new_token(l, match_char(l, '|') ? TOK_OR : TOK_ILLEGAL);

        case '<': return new_token(l, match_char(l, '=') ? TOK_LT_EQ : TOK_LT);
        case '>': return new_token(l, match_char(l, '=') ? TOK_GT_EQ : TOK_GT);

        case ',': return new_token(l, TOK_COMMA);
        case ';': return new_token(l, TOK_SEMICOLON);
        case '#': return new_token(l, TOK_COMMENT);

        case '(': return new_token(l, TOK_LPAREN);
        case ')': return new_token(l, TOK_RPAREN);
        case '{': return new_token(l, TOK_LBRACE);
        case '}': return new_token(l, TOK_RBRACE);
        case '[': return new_token(l, TOK_LBRACKET);
        case ']': return new_token(l, TOK_RBRACKET);
    }

    return new_token(l, TOK_ILLEGAL);
}

lexer_t *lexer_init(const char *src)
{
    lexer_t *l = malloc(sizeof(lexer_t));
    l->start = src;
    l->curr = src;
    l->line = 1;
    l->col = 0;

    return l;
}

void lexer_free(lexer_t *l)
{
    l->start = NULL;
    l->curr = NULL;
    free(l);
}

token_t lexer_next(lexer_t *l)
{
    return next_token(l);
}

const char *token_get_literal(token_type type)
{
    switch (type)
    {
        case TOK_IDENT: return "IDENT";
        case TOK_INT: return "INT";
        case TOK_FLOAT: return "FLOAT";
        case TOK_ASSIGN: return "=";

        case TOK_PLUS: return "+";
        case TOK_MINUS: return "-";
        case TOK_DIVIDE: return "/";
        case TOK_MULTIPLY: return "*";
        case TOK_MODULO: return "%";

        case TOK_BANG: return "!";
        case TOK_AND: return "&&";
        case TOK_OR: return "||";
        case TOK_INCREMENT: return "++";
        case TOK_DECREMENT: return "--";

        case TOK_LT: return "<";
        case TOK_GT: return ">";
        case TOK_NE: return "!=";
        case TOK_EQ: return "==";
        case TOK_LT_EQ: return "<=";
        case TOK_GT_EQ: return ">=";

        case TOK_COMMA: return ",";
        case TOK_SEMICOLON: return ";";
        case TOK_COMMENT: return "#";

        case TOK_LPAREN: return "(";
        case TOK_RPAREN: return ")";
        case TOK_LBRACE: return "{";
        case TOK_RBRACE: return "}";
        case TOK_LBRACKET: return "[";
        case TOK_RBRACKET: return "]";
        default: return "ILLEGAL";
    }
}
