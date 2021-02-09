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
    vm->cp = 0;

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
    vm->cp = 0; // TODO: This is a hack for now

    for (int i = 0; i < vm->ip; i++)
    {
        switch (vm->instructions[i])
        {
            case OP_CONST:
            {
                object_t obj = vm->constants[vm->cp++];
                push(vm, obj);

                break;
            }
            case OP_ADD:
            {
                object_t b = pop(vm);
                object_t a = pop(vm);

                if (a.type == OBJ_VAL_DOUBLE && b.type == OBJ_VAL_DOUBLE)
                {
                    a.as.double_num += b.as.double_num;

                    printf("Object add: %f\n", a.as.double_num);
                }
                else if (a.type == OBJ_VAL_LONG && b.type == OBJ_VAL_LONG)
                {
                    a.as.long_num += b.as.long_num;

                    printf("Object add: %ld\n", a.as.long_num);
                }
                else
                {
                    /* TODO: Return runtime error here */
                    printf("Invalid operands\n");
                }

                push(vm, a);
                break;
            }
            case OP_SUB:
            {
                object_t b = pop(vm);
                object_t a = pop(vm);

                if (a.type == OBJ_VAL_DOUBLE && b.type == OBJ_VAL_DOUBLE)
                {
                    a.as.double_num -= b.as.double_num;

                    printf("Object sub: %f\n", a.as.double_num);
                }
                else if (a.type == OBJ_VAL_LONG && b.type == OBJ_VAL_LONG)
                {
                    a.as.long_num -= b.as.long_num;

                    printf("Object sub: %ld\n", a.as.long_num);
                }
                else
                {
                    /* TODO: Return runtime error here */
                    printf("Invalid operands\n");
                }

                push(vm, a);
                break;
            }
            case OP_MUL:
            {
                object_t b = pop(vm);
                object_t a = pop(vm);

                if (a.type == OBJ_VAL_DOUBLE && b.type == OBJ_VAL_DOUBLE)
                {
                    a.as.double_num *= b.as.double_num;

                    printf("Object mul: %f\n", a.as.double_num);
                }
                else if (a.type == OBJ_VAL_LONG && b.type == OBJ_VAL_LONG)
                {
                    a.as.long_num *= b.as.long_num;

                    printf("Object mul: %ld\n", a.as.long_num);
                }
                else
                {
                    /* TODO: Return runtime error here */
                    printf("Invalid operands\n");
                }

                push(vm, a);
                break;
            }
            case OP_DIV:
            {
                object_t b = pop(vm);
                object_t a = pop(vm);

                if (a.type == OBJ_VAL_DOUBLE && b.type == OBJ_VAL_DOUBLE)
                {
                    a.as.double_num /= b.as.double_num;

                    printf("Object div: %f\n", a.as.double_num);
                }
                else if (a.type == OBJ_VAL_LONG && b.type == OBJ_VAL_LONG)
                {
                    a.as.long_num /= b.as.long_num;

                    printf("Object div: %ld\n", a.as.long_num);
                }
                else
                {
                    /* TODO: Return runtime error here */
                    printf("Invalid operands\n");
                }

                push(vm, a);
                break;
            }
            case OP_MOD:
            {
                object_t b = pop(vm);
                object_t a = pop(vm);

                if (a.type == OBJ_VAL_DOUBLE && b.type == OBJ_VAL_DOUBLE)
                {
                    //a.as.double_num %= b.as.double_num;
                    printf("Error: unable to modulo doubles\n");

                    //printf("Object add: %f\n", a.as.double_num);
                }
                else if (a.type == OBJ_VAL_LONG && b.type == OBJ_VAL_LONG)
                {
                    a.as.long_num %= b.as.long_num;

                    printf("Object mod: %ld\n", a.as.long_num);
                }
                else
                {
                    /* TODO: Return runtime error here */
                    printf("Invalid operands\n");
                }

                push(vm, a);
                break;
            }
            case OP_POP:
            {
                printf("OP_POP\n");
                object_t obj = pop(vm);
                print_obj(obj);

                if (obj.type == OBJ_VAL_STR) free(obj.as.str);
                break;
            }
            case OP_VAR_DECL:
            {
                object_t val = pop(vm);
                object_t ident = pop(vm);


                /* Add the value of the variable assignment to the object list */
                struct object_node *obj_val = add_obj(vm, val);

                if (!ht_contains_key(vm->globals, ident.as.str))
                {
                    ht_insert(vm->globals, ident.as.str, obj_val->obj);
                }
                else
                {
                    /* TODO: This might not be needed here. Could be put in a set var operation */
                    ht_update_key(vm->globals, ident.as.str, obj_val->obj);
                }

                break;
            }
            case OP_EXIT: break;
            default: break;
        }
    }

    printf("\nPrinting objects\n\n");
    print_obj_list(vm);
}
