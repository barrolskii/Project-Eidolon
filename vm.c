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

static void add_obj(vm_t *vm, object_t obj)
{
    if (!vm->head)
    {
        vm->head = malloc(sizeof(struct object_node));
        vm->head->next = NULL;
        vm->head->obj = malloc(sizeof(object_t));
        memcpy(vm->head->obj, &obj, sizeof(object_t));
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
    struct object_node *next = vm->head->next;

    while (curr)
    {
        if (curr->obj->type == OBJ_VAL_STR) free(curr->obj->as.str);
        free(curr->obj);
        free(curr);

        curr = next;
        next = next->next;
    }
}

vm_t *vm_init()
{
    vm_t *vm = malloc(sizeof(vm_t));
    vm->sp = 0;
    vm->ip = 0;

    vm->globals = NULL;//ht_init();

    return vm;
}

void vm_free(vm_t *vm)
{
    free_obj_list(vm);
    //ht_free(vm->globals);
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

                /* TODO: Rework the hashtable to only use a pointer to the object */
                //ht_insert(vm->globals, ident.as.str, &val);

                add_obj(vm, val);

                /* TODO: Get the garbage collector to clean these up */
                if (val.type == OBJ_VAL_STR) free(val.as.str);
                free(ident.as.str);

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
