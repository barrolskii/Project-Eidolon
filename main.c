#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "token.h"
#include "lexer.h"

int main(int argc, char **argv)
{
    char input[1024] = {0};
    lexer_t *l = NULL;
    struct token *t = NULL;

    while (1)
    {
        printf(">> ");
        scanf(" %1024[^\n]", input);

        l = lexer_init(input);

        for ( t = next_token(l); t->type != END_OF_FILE; t = next_token(l))
        {
            printf("%s -- %s\n", get_type_literal(t->type), t->literal);
            free(t->literal);
            free(t);
        }

        memset(input, 0, 1024);
    }

    return 0;
}
