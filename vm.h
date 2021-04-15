#ifndef __VM_H_
#define __VM_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "object.h"
#include "hashtable.h"

#define STACK_MAX     2048
#define CONSTANTS_MAX 64

typedef enum {
    OP_CONST    = 0,
    OP_ADD      = 1,
    OP_SUB      = 2,
    OP_MUL      = 3,
    OP_DIV      = 4,
    OP_MOD      = 5,
    OP_POP      = 6,
    OP_VAR_DECL = 7,
    OP_VAR_GET  = 8,
    OP_GT       = 9,
    OP_GT_EQ    = 10,
    OP_LT       = 11,
    OP_LT_EQ    = 12,
    OP_EQ       = 13,
    OP_NE       = 14,
    OP_IF       = 15,
    OP_ELSE     = 16,
    OP_JUMP_END = 17,
    OP_INC      = 18,
    OP_DEC      = 19,
    OP_LOOP     = 20,
    OP_LOOP_END = 21,
    OP_EXIT     = 255,
} op_code;

struct object_node {
    struct object_node *next;
    object_t *obj;
};

typedef struct {
    object_t stack[STACK_MAX];
    uint32_t sp;

    uint8_t instructions[UINT8_MAX]; /* TODO: Dynamic array */
    uint8_t ip;

    object_t constants[CONSTANTS_MAX]; /* TODO: Dynamic array */
    uint8_t cp;

    struct object_node *head;    /* List of all objects that have been allocated */
    struct hash_table *globals;
} vm_t;

vm_t *vm_init();
void vm_free(vm_t *vm);
void vm_run(vm_t *vm);

#endif // __VM_H_
