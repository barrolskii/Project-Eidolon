#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "token.h"
#include "lexer.h"


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

    lexer_t *l = lexer_init(input);
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


    lexer_t *l = lexer_init(input);
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

int main(int argc, char **argv)
{
    //TestNextToken();

    //TestTokens();

    char input[1024] = {0};
    lexer_t *l = NULL;
    struct token *t = NULL;

    while (1)
    {
        //printf(">> ");
        scanf("%s", input);

        l = lexer_init(input);

        for ( t = next_token(l); t->type != END_OF_FILE; t = next_token(l))
        {
            printf("%s -- %s\n", get_type_literal(t->type), t->literal);
            free(t);
        }

        memset(input, 0, 1024);
    }

    return 0;
}
