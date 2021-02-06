#ifndef __VM_H_
#define __VM_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "object.h"
#include "hashtable.h"

#define STACK_MAX 2048

typedef enum {
    OP_CONST,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_MOD,
    OP_POP,
    OP_VAR_DECL,
    OP_EXIT,
} op_code;

struct object_node {
    struct object_node *next;
    object_t *obj;
};

typedef struct {
    object_t stack[STACK_MAX];
    uint32_t sp;

    uint8_t instructions[UINT8_MAX];
    uint8_t ip;

    struct object_node *head;    /* List of all objects that have been allocated */
    struct hash_table *globals;
} vm_t;

vm_t *vm_init();
void vm_free(vm_t *vm);
void vm_run(vm_t *vm);

#endif // __VM_H_
