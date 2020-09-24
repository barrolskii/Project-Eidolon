#include "lexer.h"

int is_letter(char c) { return 'a' <= c && c <= 'z' || 'A' <= c && c <= 'Z' || c == '_'; }
int is_digit(char c) { return '0' <= c && c <= '9'; }


lexer_t *lexer_init(char *input)
{
    struct lexer *l = malloc(sizeof(lexer_t));
    l->input = input;
    l->pos = 0;
    l->read_pos = 0;
    l->ch = 0;

    lexer_read_char(l);

    return l;
}

void lexer_read_char(lexer_t *l)
{
    if (l->read_pos >= strlen(l->input))
    {
        l->ch = 0;
    }
    else
    {
        l->ch = l->input[l->read_pos];
    }

    l->pos = l->read_pos;
    l->read_pos++;
}

char lexer_peek_char(lexer_t *l)
{
    if (l->read_pos >= strlen(l->input))
        return 0;
    else
        return l->input[l->read_pos];
}

char *lexer_read_digit(lexer_t *l)
{
    int pos = l->pos;

    while (is_digit(l->ch))
    {
        lexer_read_char(l);
    }

    int size = (l->pos - pos) + 1;

    char *str = calloc(size, sizeof(char));
    memcpy(str, &l->input[pos], (size - 1));
    str[size - 1] = '\0';

    return str;
}

char *lexer_read_identifier(lexer_t *l)
{
    int pos = l->pos;

    while (is_letter(l->ch))
    {
        lexer_read_char(l);
    }

    int size = (l->pos - pos) + 1; // + 1 for the last character in the string and the
                                   // null termination character

    char *str = calloc(size, sizeof(char));
    memcpy(str, &l->input[pos], (size - 1));
    str[size - 1] = '\0';

    return str;
}

void lexer_skip_whitespace(lexer_t *l)
{
    while (l->ch == ' ' || l->ch == '\t' || l->ch == '\n' || l->ch == '\r')
        lexer_read_char(l);
}

struct token *next_token(lexer_t *l)
{
    struct token *tok = NULL;

    lexer_skip_whitespace(l);

    switch (l->ch)
    {
    case '=':
        if (lexer_peek_char(l) == '=')
        {
            tok = new_token(EQ, "==");
            lexer_read_char(l);
            break;
        }
        else
        {
            tok = new_token(ASSIGN, "=");
            break;
        }
    case '+':
        tok = new_token(PLUS, "+");
        break;
    case '-':
        tok = new_token(MINUS, "-");
        break;
    case '/':
        tok = new_token(DIVIDE, "/");
        break;
    case '*':
        tok = new_token(MULTIPLY, "*");
        break;
    case '%':
        tok = new_token(MODULO, "%");
        break;
    case '!':
        if (lexer_peek_char(l) == '=')
        {
            tok = new_token(NE, "!=");
            lexer_read_char(l);
            break;
        }
        else
        {
            tok = new_token(BANG, "!");
            break;
        }
    case ',':
        tok = new_token(COMMA, ",");
        break;
    case ';':
        tok = new_token(SEMI_COLON, ";");
        break;
    case '<':
        tok = new_token(LT, "<");
        break;
    case '>':
        tok = new_token(GT, ">");
        break;
    case '(':
        tok = new_token(LPAREN, "(");
        break;
    case ')':
        tok = new_token(RPAREN, ")");
        break;
    case '{':
        tok = new_token(LBRACE, "{");
        break;
    case '}':
        tok = new_token(RBRACE, "}");
        break;
    case '[':
        tok = new_token(LBRACKET, "[");
        break;
    case ']':
        tok = new_token(RBRACKET, "]");
        break;
    case 0:
        tok = new_token(END_OF_FILE, "\0");
        break;
    default:
        if (is_letter(l->ch))
        {
            tok = malloc(sizeof(struct token));
            tok->literal = lexer_read_identifier(l);
            tok->type = get_identifier(tok->literal);

            return tok;
        }
        else if (is_digit(l->ch))
        {
            tok = malloc(sizeof(struct token));
            tok->type = INT;
            tok->literal = lexer_read_digit(l);

            return tok;
        }
        else
        {
            //tok = new_token(ILLEGAL, strcat("", &l->ch));
            tok = new_token(ILLEGAL, &l->ch);
        }
    }

    lexer_read_char(l);
    return tok;
}
