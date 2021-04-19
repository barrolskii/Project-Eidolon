#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    l->col++;
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
                advance(l);
                l->line++;
                l->col = 0;
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

static token_type check_keyword(const char *start)
{
    switch (*start)
    {
        case 'b': if (memcmp(start, "break", 5) == 0) return TOK_BREAK; break;
        case 'c': if (memcmp(start, "continue", 8) == 0) return TOK_CONTINUE; break;
        case 'e': if (memcmp(start, "else", 4) == 0) return TOK_ELSE; break;
        case 'f':
        {
            if (memcmp(start, "false", 5) == 0) return TOK_FALSE;
            if (memcmp(start, "func", 4) == 0) return TOK_FUNC;
            break;
        }
        case 'i': if (memcmp(start, "if", 2) == 0) return TOK_IF; break;
        case 'l': if (memcmp(start, "loop", 4) == 0) return TOK_LOOP; break;
        case 'r': if (memcmp(start, "return", 6) == 0) return TOK_RETURN; break;
        case 's': if (memcmp(start, "stdin", 5) == 0) return TOK_STDIN; break;
        case 't': if (memcmp(start, "true", 4) == 0) return TOK_TRUE; break;
        case 'v': if (memcmp(start, "var", 3) == 0) return TOK_VAR; break;
    }

    return TOK_IDENT;
}

static token_t new_identifer(lexer_t *l)
{
    while (is_char(peek_char(l)) || is_digit(peek_char(l))) advance(l);

    token_type type = check_keyword(l->start);

    return new_token(l, type);
}

static token_t new_number(lexer_t *l)
{
    token_type type = TOK_INT;

    while (is_digit(peek_char(l)) || peek_char(l) == '.')
    {
        if (peek_char(l) == '.') type = TOK_FLOAT;
        advance(l);
    }

    return new_token(l, type);
}

static token_t new_string(lexer_t *l)
{
    while (peek_char(l) != '"' && !is_script_end(peek_char(l)))
    {
        if (peek_char(l) == '\n') l->line++;
        advance(l);
    }

    if (is_script_end(peek_char(l))) return new_token(l, TOK_ERROR);

    /*  Advance for the closing quote of the string */
    advance(l);

    return new_token(l, TOK_STRING);
}

static token_t next_token(lexer_t *l)
{
    skip_whitespace(l);

    l->start = l->curr;

    if (is_script_end(*l->curr)) return new_token(l, TOK_EOF);

    char c = advance(l);

    if (is_char(c)) return new_identifer(l);
    if (is_digit(c)) return new_number(l);

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

        case '"': return new_string(l);
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

const char *token_get_type_literal(token_type type)
{
    switch (type)
    {
        case TOK_IDENT: return "IDENT";
        case TOK_ASSIGN: return "ASSIGN";
        case TOK_INT: return "INT";
        case TOK_FLOAT: return "FLOAT";
        case TOK_STRING: return "STRING";

        case TOK_PLUS: return "PLUS";
        case TOK_MINUS: return "MINUS";
        case TOK_DIVIDE: return "DIVIDE";
        case TOK_MULTIPLY: return "MULTIPLY";
        case TOK_MODULO: return "MODULO";

        case TOK_BANG: return "BANG";
        case TOK_AND: return "AND";
        case TOK_OR: return "OR";
        case TOK_INCREMENT: return "INCREMENT";
        case TOK_DECREMENT: return "DECREMENT";

        case TOK_LT: return "LT";
        case TOK_GT: return "GT";
        case TOK_NE: return "NE";
        case TOK_EQ: return "EQ";
        case TOK_LT_EQ: return "LT_EQ";
        case TOK_GT_EQ: return "GT_EQ";

        case TOK_COMMA: return "COMMA";
        case TOK_SEMICOLON: return "SEMICOLON";
        case TOK_COMMENT: return "COMMENT";

        case TOK_LPAREN: return "LPAREN";
        case TOK_RPAREN: return "RPAREN";
        case TOK_LBRACE: return "LBRACE";
        case TOK_RBRACE: return "RBRACE";
        case TOK_LBRACKET: return "LBRACKET";
        case TOK_RBRACKET: return "RBRACKET";

        case TOK_VAR: return "VAR";
        case TOK_IF: return "IF";
        case TOK_ELSE: return "ELSE";
        case TOK_LOOP: return "LOOP";
        case TOK_FUNC: return "FUNC";
        case TOK_RETURN: return "RETURN";
        case TOK_BREAK: return "BREAK";
        case TOK_CONTINUE: return "CONTINUE";
        case TOK_TRUE: return "TRUE";
        case TOK_FALSE: return "FALSE";

        case TOK_STDIN: return "STDIN";
        default: return "ILLEGAL";
    }
}
