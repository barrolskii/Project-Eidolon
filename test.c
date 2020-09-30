#include <stdio.h>

#include "lexer.h"

#define ITR_TESTS(N)\
    lexer_t *l = lexer_init(input); \
    struct token *tok = NULL; \
\
    for (int i = 0; i < N; i++) \
    { \
        tok = next_token(l); \
\
        if (tok->type != tests[i].exp_t) \
        { \
            printf("tests[%d] - incorrect token type. Expected %s, got %s.\n", \
                   i, get_type_literal(tests[i].exp_t), get_type_literal(tok->type)); \
\
            free(tok->literal); \
            free(tok); \
            return -1; \
        } \
\
        if (strcmp(tok->literal, tests[i].literal)) \
        { \
            printf("tests[%d] - incorrect literal. Expected %s, got %s.\n", \
                   i, tests[i].literal, tok->literal); \
\
            free(tok->literal); \
            free(tok); \
            return -1; \
        } \
\
        printf("test[%d] - %20s : tok - %s\n", i, get_type_literal(tests[i].exp_t), get_type_literal(tok->type)); \
        printf("test[%d] - %20s : tok - %s\n", i, tests[i].literal, tok->literal); \
\
        free(tok->literal); \
        free(tok); \
    } \
\
    free(l); \
\
    printf("Tests passed\n"); \

typedef struct test
{
    token_type_t exp_t; /* Expected type */
    char *literal;
} test_t;

int test_ident_and_types()
{
    char *input =
            "value \
            1234 \
            123.456 \
            \"my^str$ing\" \
            'c'";

    test_t tests[] = {
        {IDENT, "value" },
        {INT, "1234" },
        {FLOAT, "123.456" },
        {STRING, "my^str$ing" },
        {CHAR, "c" },
    };

    ITR_TESTS(5)

    return 0;
}

int test_operators()
{
    char *input = "=+-/*%!&&||++--,;(){}<>[]==!=<=>=";

    test_t tests[] = {
        { ASSIGN, "=" },
        { PLUS, "+" },
        { MINUS, "-" },
        { DIVIDE, "/" },
        { MULTIPLY, "*" },
        { MODULO, "%" },
        { BANG, "!" },

        { AND, "&&" },
        { OR, "||" },
        { INCREMENT, "++" },
        { DECREMENT, "--" },

        { COMMA, "," },
        { SEMI_COLON, ";" },

        { LPAREN, "(" },
        { RPAREN, ")" },
        { LBRACE, "{" },
        { RBRACE, "}" },
        { LT, "<" },
        { GT, ">" },
        { LBRACKET, "[" },
        { RBRACKET, "]" },

        { EQ, "==" },
        { NE, "!=" },
        { LT_EQ, "<=" },
        { GT_EQ, ">=" },
    };

	ITR_TESTS(25)

    return 0;
}

int test_keywords()
{
    char *input =
        "var \
        if \
        else \
        loop \
        func \
        return \
        break \
        continue \
        true \
        false";


    test_t tests[] = {
        { VAR, "var" },
        { IF, "if" },
        { ELSE, "else" },
        { LOOP, "loop" },
        { FUNC, "func" },
        { RETURN, "return" },
        { BREAK, "break" },
        { CONTINUE, "continue" },
        { TRUE, "true" },
        { FALSE, "false" },
    };

    ITR_TESTS(9)

    return 0;
}

int main(int argc, char **argv)
{
    test_ident_and_types();
    //test_operators();
    //test_keywords();


    return 0;
}
