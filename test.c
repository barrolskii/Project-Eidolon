#include <stdio.h>
#include <string.h>

#include "lexer.h"
#include "parser.h"

typedef struct lexer_test
{
    token_type_t exp_t; /* Expected type */
    char *literal;
} lexer_test_t;

typedef struct parser_test
{
    char *exp_i; /* Expected identifier */
} parser_test_t;

int test_lexer_tokens(char *input, lexer_test_t *tests, unsigned count)
{
    lexer_t *l = lexer_init(input);
    struct token *tok = NULL;

    for (int i = 0; i < count; i++)
    {
        tok = next_token(l);

        if (tok->type != tests[i].exp_t)
        {
            printf("tests[%d] - incorrect token type. Expected %s, got %s.\n",
                   i, get_type_literal(tests[i].exp_t), get_type_literal(tok->type));

            free(tok->literal);
            free(tok);
            return -1;
        }

        if (strcmp(tok->literal, tests[i].literal))
        {
        fprintf(stderr,
                "tests[%d] - incorrect literal. Expected %s, got %s.\n",
                i, tests[i].literal, tok->literal);

            free(tok->literal);
            free(tok);
            return -1;
        }

        printf("test[%d] - %20s : tok - %s\n", i, get_type_literal(tests[i].exp_t), get_type_literal(tok->type));
        printf("test[%d] - %20s : tok - %s\n", i, tests[i].literal, tok->literal);

        free(tok->literal);
        free(tok);
    }

    free(l);


    return 0;
}

int test_parser(char *input, parser_test_t *tests, unsigned count, int (*func)(struct node *, parser_test_t *))
{
    printf("input: %s\n", input);

    lexer_t *l = lexer_init(input);
    parser_t *p = parser_init(l);

    program_t *prog = parser_parse_program(p);

    if (!prog)
    {
        fprintf(stderr, "parser_parse_program() returned NULL\n");
        goto free_memory;
        return -1;
    }

    if (prog->count != count)
    {
        fprintf(stderr,
                "prog->count does not contain %d statements. Got %d\n",
                count, prog->count);
        return -1;
    }

    struct node *curr_stmt = prog->head; /* Current statement in program */
    int i = 0;

    while(curr_stmt)
    {
        if (func(curr_stmt, &tests[i])) { return -1; }

        printf("curr_stmt: %s -- &tests[%d]: %s\n", curr_stmt->data->literal, i, tests[i].exp_i);

        curr_stmt = curr_stmt->next;
        i++;
    }

free_memory:
    free(l);
    free(p);

    return 0;
}

int test_parser_var_statement(struct node *stmt, parser_test_t *test)
{
    if (stmt->data->type != VAR)
    {
        fprintf(stderr, "Token type not VAR. Got %s\n", get_type_literal(stmt->data->type));
        return -1;
    }

    if (strcmp(stmt->data->literal, test->exp_i))
    {
       printf("%d\n", strcmp(stmt->data->literal, test->exp_i));
       fprintf(stderr, "Var statement literal not %s. Got %s\n", test->exp_i, stmt->data->literal);
       return -1;
    }

    return 0;
}

int main(int argc, char **argv)
{
    char *type_input =
            "value \
            1234 \
            123.456 \
            \"my^str$ing\" \
            'c'";

    lexer_test_t type_tests[] = {
        {IDENT, "value" },
        {INT, "1234" },
        {FLOAT, "123.456" },
        {STRING, "my^str$ing" },
        {CHAR, "c" },
    };

    char *operator_input = "=+-/*%!&&||++--,;(){}<>[]==!=<=>=";

    lexer_test_t operator_tests[] = {
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

    char *keywords_input =
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


    lexer_test_t keywords_tests[] = {
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

    //test_lexer_tokens(type_input, type_tests, 5);
    //test_lexer_tokens(operator_input, operator_tests, 25);
    //test_lexer_tokens(keywords_input, keywords_tests, 10);

    char *parser_input =
        "var x = 5; \
         var y = 10; \
         var foobar = 123456;";

    parser_test_t parser_expected[] = {
        { "x" },
        { "y" },
        { "foobar" }
    };

    test_parser(parser_input, parser_expected, 3, test_parser_var_statement);

    return 0;
}
