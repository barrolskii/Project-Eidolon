#include "ast.h"

program_t *ast_init_program()
{
    program_t *prog = malloc(sizeof(program_t));
    prog->head = NULL;
    prog->count = 0;

    return prog;
}


int ast_append_node(program_t *prog, statement_t *stmt)
{
    if (!prog->head)
    {
        prog->head = malloc(sizeof(struct node));
        prog->head->data = stmt;
        prog->head->next = NULL;

        prog->count++;

        return 0;
    }

    struct node *curr = prog->head;
    while (curr->next)
    {
        curr = curr->next;
    }

    curr->next = malloc(sizeof(struct node));
    curr->next->data = stmt;
    curr->next->next = NULL;
    prog->count++;

    return 0;
}
