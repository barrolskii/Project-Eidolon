#include "vm.h"

static object_t pop(vm_t *vm)
{
    object_t obj = vm->stack[vm->sp - 1];
    vm->sp--;

    return obj;
}

static void push(vm_t *vm, object_t obj)
{
    if (vm->sp >= STACK_MAX)
    {
        fprintf(stderr, "Error: Stack overflow\n");
        return;
    }

    vm->stack[vm->sp] = obj;
    vm->sp++;
}

static void exec_binary_op(vm_t *vm)
{
    op_code code = vm->instructions[vm->ip];

    switch (code)
    {
        case OP_ADD:
        {
            object_t b = pop(vm);
            object_t a = pop(vm);
            a.as.long_num += b.as.long_num;
            push(vm, a);
            break;
        }
        case OP_SUB:
        {
            object_t b = pop(vm);
            object_t a = pop(vm);
            a.as.long_num -= b.as.long_num;
            push(vm, a);
            break;
        }
        case OP_MUL:
        {
            object_t b = pop(vm);
            object_t a = pop(vm);
            a.as.long_num *= b.as.long_num;
            push(vm, a);
            break;
        }
        case OP_DIV:
        {
            object_t b = pop(vm);
            object_t a = pop(vm);
            a.as.long_num /= b.as.long_num;
            push(vm, a);
            break;
        }
        case OP_MOD:
        {
            object_t b = pop(vm);
            object_t a = pop(vm);
            a.as.long_num %= b.as.long_num;
            push(vm, a);
            break;
        }
        default:
            printf("Other binary operations not yet implemented\n");
    }
}

vm_t *vm_init()
{
    vm_t *vm = malloc(sizeof(vm_t));
    //vm->stack = calloc(STACK_MAX, sizeof(object_t));
    vm->sp = 0;
    //vm->constants = NULL;
    //vm->instructions = calloc(UINT8_MAX, sizeof(uint8_t));
    vm->ip = 0;

    vm->const_count = 0;
    vm->ci = 0;

    vm->instruct_count = 0;

    vm->globals = ht_init();

    return vm;
}

void vm_free(vm_t *vm)
{
    //free(vm->stack);
    //free(vm->instructions);

    ht_free(vm->globals);
    free(vm);
}

void vm_run(vm_t *vm)
{
    while (vm->ip < vm->instruct_count)
    {
        op_code code = vm->instructions[vm->ip];

        switch (code)
        {
            case OP_CONST:
            {
                object_t obj = vm->constants[vm->ci];
                push(vm, obj);
                vm->ci++;
                break;
            }
            case OP_ADD:
            case OP_SUB:
            case OP_MUL:
            case OP_DIV:
            case OP_MOD:
                exec_binary_op(vm); break;
            case OP_POP:
            {
                object_t obj = pop(vm);
                printf("%ld\n", obj.as.long_num);
                break;
            }
            case OP_ADD_GLOBAL:
            {
                object_t obj_b = pop(vm);
                object_t *obj_val = malloc(sizeof(object_t));
                memcpy(obj_val, &obj_b, sizeof(object_t));

                object_t obj_a = pop(vm);
                ht_insert(vm->globals, obj_a.as.str, obj_val);
                int val = ht_contains_key(vm->globals, "a");
                printf("val: %d\n", val);

                break;
            }
            case OP_GET_GLOBAL:
            {
                printf("Get global inst\n");

                object_t obj = pop(vm);

                int val = ht_contains_key(vm->globals, obj.as.str);
                if (!val)
                {
                    /* Runtime error */
                    printf("Error: Undefined variable \n");
                }

                object_t obj_two;
                ht_get_value(vm->globals, obj.as.str, &obj_two);
                push(vm, obj_two);

                break;
            }
            default:
                printf("Default reached\n");
        }

        vm->ip++;
    }
}
