#include "vm.h"

static object_t pop(vm_t *vm)
{
    return vm->stack[--vm->sp];
}

static void push(vm_t *vm, object_t obj)
{
    vm->stack[vm->sp++] = obj;
}

static void print_obj(object_t obj)
{
    if (obj.type == OBJ_VAL_DOUBLE)
        printf("obj: %f\n", obj.as.double_num);
    else if (obj.type == OBJ_VAL_LONG)
        printf("obj: %ld\n", obj.as.long_num);
    else
        printf("obj: %s\n", obj.as.str);
}

static struct object_node *add_obj(vm_t *vm, object_t obj)
{
    if (!vm->head)
    {
        vm->head = malloc(sizeof(struct object_node));
        vm->head->next = NULL;
        vm->head->obj = malloc(sizeof(object_t));
        memcpy(vm->head->obj, &obj, sizeof(object_t));

        return vm->head;
    }

    struct object_node *curr = vm->head;
    while (curr->next)
    {
        curr = curr->next;
    }

    curr->next = malloc(sizeof(struct object_node));
    curr->next->next = NULL;
    curr->next->obj = malloc(sizeof(object_t));
    memcpy(curr->next->obj, &obj, sizeof(object_t));

    return curr->next;
}

static void print_obj_list(vm_t *vm)
{
    struct object_node *curr = vm->head;

    while (curr)
    {
        print_obj(*curr->obj);
        curr = curr->next;
    }
}

static void free_obj_list(vm_t *vm)
{
    struct object_node *curr = vm->head;
    struct object_node *next = vm->head;

    while (next)
    {
        next = curr->next;

        if (curr->obj->type == OBJ_VAL_STR) free(curr->obj->as.str);
        free(curr->obj);
        free(curr);

        curr = next;
    }
}

vm_t *vm_init()
{
    vm_t *vm = malloc(sizeof(vm_t));
    vm->sp = 0;
    vm->ip = 0;

    vm->globals = ht_init();
    vm->head = NULL;

    return vm;
}

void vm_free(vm_t *vm)
{
    free_obj_list(vm);
    ht_free(vm->globals);
    free(vm);
}

void vm_run(vm_t *vm)
{
    printf("\n\nRunning vm\n");
    printf("ip: %d\n", vm->ip);
    printf("sp: %d\n", vm->sp);

    for (int i = 0; i < vm->ip; i++)
    {
        switch (vm->instructions[i])
        {
            case OP_VAR_DECL:
            {
                object_t val = pop(vm);
                object_t ident = pop(vm);

                //print_obj(val);
                //print_obj(ident);

                /* Add the value of the variable assignment to the object list */
                struct object_node *obj_val = add_obj(vm, val);
                ht_insert(vm->globals, ident.as.str, obj_val->obj);

                printf("Contains [foo]: %d\n", ht_contains_key(vm->globals, "foo"));

                break;
            }
            case OP_EXIT:
                /* TODO: Cleanup here */
                break;

            default: break;
        }
    }

    printf("\nPrinting objects\n\n");
    print_obj_list(vm);
}
