#include "vm.h"

#define BINARY_OP(vm, op)                       \
     object_t b           = pop(vm);            \
     object_t a           = pop(vm);            \
                                                \
     if (a.type == OBJ_VAL_DOUBLE && b.type == OBJ_VAL_DOUBLE) \
     {                                          \
         a.as.double_num = a.as.double_num op b.as.double_num;    \
     }                                          \
     else if (a.type == OBJ_VAL_LONG && b.type == OBJ_VAL_LONG) \
     {                                          \
         a.as.long_num = a.as.long_num op b.as.long_num;      \
     }                                          \
     else                                       \
     {                                          \
         printf("Invalid operands\n"); /* TODO: Return runtime error here */ \
     }                                          \
                                                \
     push(vm, a);


/* TODO: Clean this up */
#define COMPARE_OBJS(vm, op)                    \
    object_t b = pop(vm);                       \
    object_t a = pop(vm);                       \
                                                \
    if (a.type == OBJ_VAL_LONG && b.type == OBJ_VAL_LONG) \
        a.as.long_num op b.as.long_num ? push(vm, obj_true) : push(vm, obj_false); \
    else if (a.type == OBJ_VAL_DOUBLE && b.type == OBJ_VAL_DOUBLE) \
        a.as.double_num op b.as.double_num ? push(vm, obj_true) : push(vm, obj_false); \
    else if (a.type == OBJ_VAL_LONG && b.type == OBJ_VAL_DOUBLE) \
        a.as.long_num op b.as.double_num ? push(vm, obj_true) : push(vm, obj_false); \
    else if (a.type == OBJ_VAL_DOUBLE && b.type == OBJ_VAL_LONG) \
        a.as.double_num op b.as.long_num ? push(vm, obj_true) : push(vm, obj_false); \
    else                                        \
        push(vm, obj_false)     // TODO: For now comparing incombatible types yields false

static object_t obj_true = {
    .type = OBJ_VAL_BOOL,
    .as.str = "true"
};

static object_t obj_false = {
    .type = OBJ_VAL_BOOL,
    .as.str = "false"
};

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
        printf("%f\n", obj.as.double_num);
    else if (obj.type == OBJ_VAL_LONG)
        printf("%ld\n", obj.as.long_num);
    else
        printf("%s\n", obj.as.str);
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

    if (obj.type == OBJ_VAL_STR)
    {
        curr->obj->as.str = obj.as.str;
    }

    return curr->next;
}

static int add_obj_ptr(vm_t *vm, object_t *obj)
{
    if (!vm->head)
    {
        vm->head = malloc(sizeof(struct object_node));
        vm->head->next = NULL;
        vm->head->obj = obj;

        return 1;
    }

    struct object_node* curr = vm->head;
    while (curr->next)
    {
        curr = curr->next;
    }

    curr->next = malloc(sizeof(struct object_node));
    curr->next->next = NULL;
    curr->next->obj = obj;

    return 1;
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

        if (curr->obj->type == OBJ_VAL_STR && *curr->obj->as.str != NULL)
        {
            free(curr->obj->as.str);
        }
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
                BINARY_OP(vm, +);
                break;
            }
            case OP_SUB:
            {
                BINARY_OP(vm, -);
                break;
            }
            case OP_MUL:
            {
                BINARY_OP(vm, *);
                break;
            }
            case OP_DIV:
            {
                BINARY_OP(vm, /);
                break;
            }
            case OP_MOD:
            {
                /* We can't use the BINARY_OP macro with the modulus operator
                *  just because of how it works in C. You can't use the
                *  modulus operator with floating point values
                */
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
                object_t obj = pop(vm);
                print_obj(obj);

                break;
            }
            case OP_VAR_DECL:
            {
                object_t val = pop(vm);
                object_t ident = pop(vm);

                object_t *heap_val = malloc(sizeof(object_t));
                memcpy(heap_val, &val, sizeof(object_t));

                add_obj_ptr(vm, heap_val);

                if (!ht_contains_key(vm->globals, ident.as.str))
                {
                    ht_insert(vm->globals, ident.as.str, heap_val);
                }
                else
                {
                    /* TODO: This might not be needed here. Could be put in a set var operation */
                    ht_update_key(vm->globals, ident.as.str, heap_val);
                }

                break;
            }
            case OP_VAR_GET:
            {
                object_t ident = pop(vm);

                if (!ht_contains_key(vm->globals, ident.as.str))
                {
                    printf("Error: variable '%s' not declared\n", ident.as.str);
                    free(ident.as.str);

                    /* TODO: For now skip the next pop operation but in future return a
                     * runtime error and exit gracefully
                     */
                     i++;
                }
                else
                {
                    object_t *val = ht_get_value(vm->globals, ident.as.str);
                    push(vm, *val);
                    //print_obj(*val);
                    //free(ident.as.str); /* TODO: Garbage collector here? */
                }
                break;
            }
            case OP_GT:
            {
                /* TODO: See if you can make this a function. If not then leave as is */
                COMPARE_OBJS(vm, >);
                break;
            }
            case OP_GT_EQ:
            {
                COMPARE_OBJS(vm, >=);
                break;
            }
            case OP_LT:
            {
                COMPARE_OBJS(vm, <);
                break;
            }
            case OP_LT_EQ:
            {
                COMPARE_OBJS(vm, <=);
                break;
            }
            case OP_EQ:
            {
                COMPARE_OBJS(vm, ==);
                break;
            }
            case OP_NE:
            {
                COMPARE_OBJS(vm, !=);
                break;
            }
            case OP_IF:
            {
                // Peek the item on the stack
                object_t obj = vm->stack[vm->sp];

                if (obj.type == OBJ_VAL_BOOL && strcmp(obj.as.str, "true") == 0)
                    break;

                /* TODO: Make this a bit cleaner but for now it just works */
                if (obj.type == OBJ_VAL_LONG && obj.as.long_num != 0) 
                    break;

                if (obj.type == OBJ_VAL_DOUBLE && obj.as.double_num != 0)
                    break;

                if (obj.type == OBJ_VAL_STR && obj.as.str)
                    break;

                //if (obj.type != OBJ_VAL_BOOL && (obj.as.str || obj.as.long_num != 0 || obj.as.double_num != 0))
                    //break;


                // If not then skip out of the statement
                printf("Skipping\n");
                while (1)
                {
                    if (vm->instructions[i + 1] == OP_CONST) vm->cp++;

                    if (vm->instructions[i + 1] == OP_JUMP_END ||
                        vm->instructions[i + 1] == OP_ELSE)
                    {
                        i++;
                        break;
                    }

                    i++;
                }
                

                break;
            }
            case OP_ELSE:
            {
                printf("OP_ELSE\n");

                while (vm->instructions[i] != OP_JUMP_END)
                    i++;

                break;
            }
            case OP_JUMP_END:
            {
                //printf("OP_JUMP_END\n");
                break;
            }
            case OP_INC:
            {
                /* TODO: Make this a function */
                object_t ident = pop(vm);

                if (!ht_contains_key(vm->globals, ident.as.str))
                {
                    printf("Error: variable '%s' not declared\n", ident.as.str);
                }
                else
                {
                    object_t *val = ht_get_value(vm->globals, ident.as.str);

                    if (val->type == OBJ_VAL_LONG)
                        val->as.long_num++;
                    else if (val->type == OBJ_VAL_DOUBLE)
                        val->as.double_num++;

                    push(vm, *val);
                    free(ident.as.str); /* TODO: Garbage collector here? */
                }

                break;
            }
            case OP_DEC:
            {
                object_t ident = pop(vm);

                if (!ht_contains_key(vm->globals, ident.as.str))
                {
                    printf("Error: variable '%s' not declared\n", ident.as.str);
                }
                else
                {
                    object_t *val = ht_get_value(vm->globals, ident.as.str);

                    if (val->type == OBJ_VAL_LONG)
                        val->as.long_num--;
                    else if (val->type == OBJ_VAL_DOUBLE)
                        val->as.double_num--;

                    push(vm, *val);
                    free(ident.as.str); /* TODO: Garbage collector here? */
                }

                break;
            }
            case OP_LOOP:
            {
                /* Check the expression on top of the stack */
                object_t obj = vm->stack[vm->sp - 1];

                static unsigned orig_cp = 0;
                static unsigned end_cp = 0;

                if (orig_cp == 0)
                    orig_cp = vm->cp;
                else
                {
                    if (end_cp == 0)
                        end_cp = vm->cp;

                    vm->cp = orig_cp;
                }

                if (obj.as.str || obj.as.double_num != 0 || obj.as.long_num != 0)
                {
                    // If it is then execute the expression
                    // Decrement the expression so we can leave the loop
                    vm->stack[vm->sp - 1].as.long_num--;
                }
                else
                {
                    // If not then skip out of the statement
                    while (vm->instructions[i] != OP_LOOP_END)
                        i++;

                    //vm->cp = orig_cp + 1;
                    vm->cp = end_cp;

                    // TODO: Make constants cleanup function!!!
                    //if (vm->constants[orig_cp].type == OBJ_VAL_STR)
                        //free(vm->constants[orig_cp].as.str);
                }

                break;
            }
            case OP_LOOP_END:
            {
                while(vm->instructions[i] != OP_LOOP)
                    i--;

                /*
                 * Decrement i once more so we move into that instruction
                 * on the next iteration
                 */
                i--;

                break;
            }
            case OP_STDIN:
            {
                object_t obj;
                char buffer[1024] = {0};

                scanf(" %1024s", buffer);

                long str_long = atol(buffer);

                if (buffer[0] == '0' || str_long != 0)
                {
                    obj.as.long_num = str_long;
                    obj.type = OBJ_VAL_LONG;
                }
                else
                {
                    obj.as.str = malloc(strlen(buffer) + 1);
                    strcpy(obj.as.str, buffer);


                    obj.type = OBJ_VAL_STR;
                }


                push(vm, obj);

                break;
            }
            case OP_RAND:
            {
                object_t range = pop(vm);
                object_t val = { .as.long_num = rand() % range.as.long_num, .type = OBJ_VAL_LONG };

                push(vm, val);

                break;
            }
            case OP_EXIT: return;
            default: break;
        }
    }

    //print_obj_list(vm);
}
