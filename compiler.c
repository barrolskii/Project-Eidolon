#include "compiler.h"

static void emit_byte(vm_t *vm, op_code code)
{
    vm->instructions[vm->ip] = code;
    vm->ip++;
}

static void push_obj(vm_t *vm, object_t obj)
{
    vm->stack[vm->sp] = obj;
    vm->sp++;
}

static void compile_var(compiler_t *c, expr_t *expr)
{
    /* Left token is identifier so this is always a string object */
    char *ident = malloc(sizeof(expr->left->tok.len));
    memcpy(ident, expr->left->tok.start, expr->left->tok.len);
    ident[expr->left->tok.len] = '\0';
    object_t ident_obj = { .type = OBJ_VAL_STR, .as.str = ident };
    printf("ident obj: %s\n", ident_obj.as.str);

    /* Right token is the value */
    object_t val_obj;

    /* TODO: Update the tokens to reflect long and double instead of float and int */
    if (expr->right->tok.type == TOK_FLOAT)
    {
        val_obj.type = OBJ_VAL_DOUBLE;
        val_obj.as.double_num = strtod(expr->right->tok.start, NULL);
        printf("val obj double: %f\n", val_obj.as.double_num);
    }
    else if (expr->right->tok.type == TOK_INT)
    {
        val_obj.type = OBJ_VAL_LONG;
        val_obj.as.long_num = strtol(expr->right->tok.start, NULL, 10);
        printf("val obj long: %ld\n", val_obj.as.long_num);
    }
    else
    {
        val_obj.type = OBJ_VAL_STR;

        char *str = malloc(sizeof(char) * expr->right->tok.len);
        memcpy(str, expr->right->tok.start, expr->right->tok.len);
        str[expr->right->tok.len] = '\0';

        val_obj.as.str = str;
        printf("val obj str: %s\n", val_obj.as.str);
    }

    push_obj(c->vm, ident_obj);
    push_obj(c->vm, val_obj);
    emit_byte(c->vm, OP_VAR_DECL);
}

static int compile_expr(compiler_t *c, expr_t *expr)
{
    /* Empty expression */
    if (!expr) return 0;

    switch (expr->tok.type)
    {
        case TOK_ASSIGN:
        {
            compile_var(c, expr);
            break;
        }

        default: break;
    }

    printf("compile_expr type: %s\n", token_get_type_literal(expr->tok.type));

    return 1;
}

compiler_t *compiler_init(vm_t *vm)
{
    compiler_t *c = malloc(sizeof(compiler_t));
    c->vm = vm;

    return c;
}

void compiler_free(compiler_t *c)
{
    free(c);
}

compiler_code_t compiler_compile_program(compiler_t *c, ast_node_t *ast)
{
    ast_node_t *curr = ast;

    //ast_node_print_header();
    //ast_node_print_node(curr);

    while (curr)
    {
        printf("Curr node: %s\n", token_get_type_literal(curr->expr->tok.type));
        compile_expr(c, curr->expr);

        curr = curr->next;
    }

    emit_byte(c->vm, OP_EXIT);

    return COMPILER_OK;
}
