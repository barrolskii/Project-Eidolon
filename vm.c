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
    printf("Exec binary op\n");

    op_code code = vm->instructions[vm->ip - 1];

    switch (code)
    {
        case OP_ADD:
        {
            object_t b = pop(vm);
            object_t a = pop(vm);
            a.as.long_num += b.as.long_num;
            push(vm, a);
            vm->ip--;
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
    vm->constants = NULL;
    //vm->instructions = calloc(UINT8_MAX, sizeof(uint8_t));
    vm->ip = 0;

    return vm;
}

void vm_free(vm_t *vm)
{
    //free(vm->stack);
    //free(vm->instructions);
    free(vm);
}

void vm_run(vm_t *vm)
{
    while (vm->ip > 0)
    {
        switch (vm->instructions[vm->ip - 1])
        {
            case OP_CONST:
            {
                object_t obj = pop(vm);
                printf("%ld \n", obj.as.long_num);
                break;
            }
            case OP_ADD:
                exec_binary_op(vm); break;
            default:
                printf("Default reached\n");
        }

        vm->ip--;
    }
}
