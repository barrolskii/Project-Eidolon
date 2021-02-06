#ifndef __COMPILER_H_
#define __COMPILER_H_

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "object.h"
#include "ast.h"
#include "vm.h"

typedef enum {
    COMPILER_PARSE_ERROR,
    COMPILER_RUNTIME_ERROR,
    COMPILER_OK,
} compiler_code_t;

typedef struct {
    vm_t *vm; /* Reference to the vm to push objects and instructions to */
    uint32_t scope;
} compiler_t;

compiler_t *compiler_init(vm_t *vm);
void compiler_free(compiler_t *c);
compiler_code_t compiler_compile_program(compiler_t *c, ast_node_t *ast);

#endif // __COMPILER_H_
