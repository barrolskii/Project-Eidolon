#ifndef __PHANTOM_VM_H_
#define __PHANTOM_VM_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "lexer.h"
#include "object.h"

#define STACK_MAX 2048

typedef enum {
    OP_CONST,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_MOD,
    OP_POP,
} op_code;

typedef enum {
    OBJ_VAL_LONG,
} object_value_t;

typedef struct {
    object_value_t type;
    union {
        long long_num;
        double double_num;
        char *str;
    } as;

} object_t;

typedef struct {
    //object_t *stack;
    object_t stack[STACK_MAX];
    uint32_t sp;            /* Stack pointer */

    object_t constants[STACK_MAX];
    uint32_t ci;            /* Constant index */
    uint32_t const_count;   /* Total constants */

    //object_t *constants;
    //uint8_t *instructions;
    //
    uint8_t instructions[UINT8_MAX];
    uint32_t ip;             /* Instruction pointer */
    uint32_t instruct_count;
} vm_t;


vm_t *vm_init();
void vm_free(vm_t *vm);
void vm_run(vm_t *vm);

#endif // __PHANTOM_VM_H_
