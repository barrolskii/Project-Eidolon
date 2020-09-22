#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "token.h"


/* Lexer */
struct lexer {
    char* input;
    int pos;
    int read_pos; /* Current reading position */
    char ch;
};

int is_letter(char c) { return 'a' <= c && c <= 'z' || 'A' <= c && c <= 'Z' || c == '_'; }
int is_digit(char c) { return '0' <= c && c <= '9'; }

void read_char(struct lexer* l)
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

char peek_char(struct lexer *l)
{
    if (l->read_pos >= strlen(l->input))
        return 0;
    else
        return l->input[l->read_pos];
}

char *read_digit(struct lexer *l)
{
    int pos = l->pos;

    while (is_digit(l->ch))
    {
        read_char(l);
    }

    int size = (l->pos - pos) + 1;

    char *str = calloc(size, sizeof(char));
    memcpy(str, &l->input[pos], (size - 1));
    str[size - 1] = '\0';

    return str;
}

char *lexer_read_identifier(struct lexer *l)
{
    int pos = l->pos;

    while (is_letter(l->ch))
    {
        read_char(l);
    }

    int size = (l->pos - pos) + 1; // + 1 for the last character in the string and the
                                   // null termination character

    char *str = calloc(size, sizeof(char));
    memcpy(str, &l->input[pos], (size - 1));
    str[size - 1] = '\0';

    return str;
}


struct lexer* init_lexer(char *input)
{
    struct lexer* l = malloc(sizeof(struct lexer));
    l->input = input;
    l->pos = 0;
    l->read_pos = 0;
    l->ch = 0;

    read_char(l);

    return l;
}


struct token* new_token(token_type_t t, char *literal)
{
    struct token* tok = malloc(sizeof(struct token));
    tok->type = t;
    tok->literal = malloc(sizeof(char) * strlen(literal) + 1);
    strcpy(tok->literal, literal);
    //tok->literal[0] = literal;
    //tok->literal[strlen(literal) + 1] = '\0';

    return tok;
}

void lexer_skip_whitespace(struct lexer *l)
{
    while (l->ch == ' ' || l->ch == '\t' || l->ch == '\n' || l->ch == '\r')
        read_char(l);
}

struct token* next_token(struct lexer* l)
{
    struct token* tok = NULL;

    lexer_skip_whitespace(l);

    switch (l->ch)
    {
    case '=':
        if (peek_char(l) == '=')
        {
            tok = new_token(EQ, "==");
            read_char(l);
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
        if (peek_char(l) == '=')
        {
            tok = new_token(NE, "!=");
            read_char(l);
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
            tok->literal = read_digit(l);

            return tok;
        }
        else
        {
            //tok = new_token(ILLEGAL, strcat("", &l->ch));
            tok = new_token(ILLEGAL, &l->ch);
        }
    }

    read_char(l);
    return tok;
}

int TestTokens()
{
    char *input = "var five = 5; \
        var ten = 10; \
        var add = func(x, y) { \
        x + y; \
        }; \
        var result = add(five, ten); \
        !-/*5; \
        5 < 10 > 5; \
        if (5 < 10) { \
            return true; \
        } else { \
            return false; \
        } \
        10 == 10; \
        10 != 9;";

    struct test {
        token_type_t type;
        char *expected_literal;
    } tests[] = {
    { VAR , "var"},
    { IDENT, "five"},
    { ASSIGN, "="},
    { INT, "5"},
    { SEMI_COLON, ";"},
    { VAR , "var"},
    { IDENT, "ten"},
    { ASSIGN, "="},
    { INT, "10"},
    { SEMI_COLON, ";"},
    { VAR , "var"},
    { IDENT, "add"},
    { ASSIGN, "="},
    { FUNC, "func"},
    { LPAREN, "("},
    { IDENT, "x"},
    { COMMA, ","},
    { IDENT, "y"},
    { RPAREN, ")"},
    { LBRACE, "{"},
    { IDENT, "x"},
    { PLUS, "+"},
    { IDENT, "y"},
    { SEMI_COLON, ";"},
    { RBRACE, "}"},
    { SEMI_COLON, ";"},
    { VAR , "var"},
    { IDENT, "result"},
    { ASSIGN, "="},
    { IDENT, "add"},
    { LPAREN, "("},
    { IDENT, "five"},
    { COMMA, ","},
    { IDENT, "ten"},
    { RPAREN, ")"},
    { SEMI_COLON, ";"},
    { BANG, "!"},
    { MINUS, "-"},
    { DIVIDE, "/"},
    { MULTIPLY, "*"},
    { INT, "5"},
    { SEMI_COLON, ";"},
    { INT, "5"},
    { LT, "<"},
    { INT, "10"},
    { GT, ">"},
    { INT, "5"},
    { SEMI_COLON, ";"},
    { IF, "if"},
    { LPAREN, "("},
    { INT, "5"},
    { LT, "<"},
    { INT, "10"},
    { RPAREN, ")"},
    { LBRACE, "{"},
    { RETURN, "return"},
    { TRUE, "true"},
    { SEMI_COLON, ";"},
    { RBRACE, "}"},
    { ELSE, "else"},
    { LBRACE, "{"},
    { RETURN, "return"},
    { FALSE, "false"},
    { SEMI_COLON, ";"},
    { RBRACE, "}"},
    { INT, "10"},
    { EQ, "=="},
    { INT, "10"},
    { SEMI_COLON, ";"},
    { INT, "10"},
    { NE, "!="},
    { INT, "9"},
    { SEMI_COLON, ";"},
    { END_OF_FILE, ""},
};

    struct lexer *l = init_lexer(input);
    struct token *tok = NULL;

    for (int i = 0; i < 74; i++)
    {
        tok = next_token(l);

        if (tok->type != tests[i].type)
        {
            printf("tests[%d] - incorrect token type. Expected %s, got %s.\n",
                   i, get_type_literal(tests[i].type), get_type_literal(tok->type));

            free(tok->literal);
            free(tok);
            return -1;
        }

        if (strcmp(tok->literal, tests[i].expected_literal))
        {
            printf("tests[%d] - incorrect literal. Expected %s, got %s.\n",
                   i, tests[i].expected_literal, tok->literal);

            free(tok->literal);
            free(tok);
            return -1;
        }

        printf("test[%d] - %s : tok - %s\n", i, get_type_literal(tests[i].type), get_type_literal(tok->type));
        printf("test[%d] - %s : tok - %s\n", i, tests[i].expected_literal, tok->literal);

        free(tok->literal);
        free(tok);
    }

    free(l);

    printf("Tests passed\n");

    return 0;
}

int TestNextToken()
{
    char *input = "=+(){},;";

    struct test {
        token_type_t type;
        char *expected_literal;
    } tests[] = {
        { ASSIGN, "=" },
        { PLUS, "+" },
        { LPAREN, "(" },
        { RPAREN, ")" },
        { LBRACE, "{" },
        { RBRACE, "}" },
        { COMMA, "," },
        { SEMI_COLON, ";" },
        { END_OF_FILE, ""}
    };


    struct lexer *l = init_lexer(input);
    struct token *tok;


    for (int i = 0; i < 8; i++)
    {
        tok = next_token(l);

        if (tok->type != tests[i].type)
        {
            printf("tests[%d] - incorrect token type. Expected %s, got %s\n", i, get_type_literal(tests[i].type), get_type_literal(tok->type));
            free(tok);
            return -1;
        }


        printf("test[%d] - %s : tok - %s\n", i, get_type_literal(tests[i].type), get_type_literal(tok->type));

        free(tok);
    }

    free(l);


    return 0;
}

int main(int argc, char** argv)
{
    //TestNextToken();

    //TestTokens();

    char input[1024] = {0};
    struct lexer *l = NULL;
    struct token *t = NULL;

    while (1)
    {
        printf(">> ");
        scanf("%s", input);

        l = init_lexer(input);

        for ( t = next_token(l); t->type != END_OF_FILE; t = next_token(l))
        {
            printf("%s -- %s\n", get_type_literal(t->type), t->literal);
            free(t);
        }

        memset(input, 0, 1024);
    }

    return 0;
}
