#include "compiler.h"

static void emit_byte(vm_t *vm, op_code code)
{
    vm->instructions[vm->ip] = code;
    vm->ip++;
}

/* TODO: Fix this */
static void emit_bytes(vm_t *vm, int num_codes, op_code code, ...)
{
    va_list list;

    va_start(list, code);

    for (int i = 0; i < num_codes; i++)
        emit_byte(vm, va_arg(list, op_code));

    va_end(list);
}

static void add_obj(vm_t *vm, object_t obj)
{
    vm->constants[vm->cp] = obj;
    vm->cp++;
}

static void compile_num(compiler_t *c, expr_t *expr)
{
    object_t num = { .type = OBJ_VAL_LONG, .as.long_num = strtol(expr->tok.start, NULL, 10) };
    add_obj(c->vm, num);
    emit_byte(c->vm, OP_CONST);
}

static void compile_double(compiler_t *c, expr_t *expr)
{
    object_t num = { .type = OBJ_VAL_DOUBLE, .as.double_num = strtod(expr->tok.start, NULL)};
    add_obj(c->vm, num);
    emit_byte(c->vm, OP_CONST);
}

static void compile_string(compiler_t *c, expr_t *expr)
{
    char *str_val = malloc(sizeof(char) * expr->tok.len + 1);
    memcpy(str_val, expr->tok.start, expr->tok.len);
    str_val[expr->tok.len] = '\0';

    object_t str = { .type = OBJ_VAL_STR, .as.str = str_val };

    add_obj(c->vm, str);
    emit_byte(c->vm, OP_CONST);
}

static void compile_ident(compiler_t *c, expr_t *expr)
{
    char *str_val = malloc(sizeof(char) * expr->tok.len + 1);
    memcpy(str_val, expr->tok.start, expr->tok.len);
    str_val[expr->tok.len] = '\0';

    object_t str = { .type = OBJ_VAL_STR, .as.str = str_val };
    add_obj(c->vm, str);
    emit_byte(c->vm, OP_CONST);
    emit_byte(c->vm, OP_VAR_GET);
}

static void compile_bin_expr(compiler_t *c, expr_t *expr)
{
    if (expr->left) compile_bin_expr(c, expr->left);
    if (expr->right) compile_bin_expr(c, expr->right);

    switch (expr->tok.type)
    {
        case TOK_INT: compile_num(c, expr); break;
        case TOK_FLOAT: compile_double(c, expr); break;
        case TOK_STRING: printf("string support not added for binary ops\n"); break;

        case TOK_PLUS: emit_byte(c->vm, OP_ADD); break;
        case TOK_MINUS: emit_byte(c->vm, OP_SUB); break;
        case TOK_MULTIPLY: emit_byte(c->vm, OP_MUL); break;
        case TOK_DIVIDE: emit_byte(c->vm, OP_DIV); break;
        case TOK_MODULO: emit_byte(c->vm, OP_MOD); break;
        default:
            /* TODO: Error here */
            break;
    }
}

static void compile_var(compiler_t *c, expr_t *expr)
{
    /* Left token is identifier so this is always a string object */
    compile_string(c, expr->left);


    /* TODO: Update the tokens to reflect long and double instead of float and int */
    switch (expr->right->tok.type)
    {
        case TOK_PLUS:
        case TOK_MINUS:
        case TOK_MULTIPLY:
        case TOK_DIVIDE:
        case TOK_MODULO:
            compile_bin_expr(c, expr->right);
            break;

        case TOK_FLOAT:
        {
            compile_double(c, expr->right);
            break;
        }
        case TOK_INT:
        {
            compile_num(c, expr->right);
            break;
        }
        case TOK_STRING:
        {
            compile_string(c, expr->right);
            break;
        }
        case TOK_IDENT:
            compile_ident(c, expr->right);
            //emit_byte(c->vm, OP_VAR_GET);
            break;

        default:
           break;
    }

    emit_byte(c->vm, OP_VAR_DECL);
}

static void compile_var_get(compiler_t *c, expr_t *expr)
{
    char *ident = malloc(sizeof(char) * expr->tok.len + 1);
    memcpy(ident, expr->tok.start, expr->tok.len);
    ident[expr->tok.len] = '\0';

    object_t ident_obj = { .type = OBJ_VAL_STR, .as.str = ident };

    add_obj(c->vm, ident_obj);

    emit_byte(c->vm, OP_CONST);
    emit_byte(c->vm, OP_VAR_GET);
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
        case TOK_IDENT:
        {
            /* TODO: Look into making this one call from the compile var function */
            compile_var_get(c, expr);
            emit_byte(c->vm, OP_POP);
            break;
        }
        case TOK_PLUS:
        case TOK_MINUS:
        case TOK_MULTIPLY:
        case TOK_DIVIDE:
        case TOK_MODULO:
        {
            compile_bin_expr(c, expr);
            emit_byte(c->vm, OP_POP);
            break;
        }
        default: break;
    }

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
        //printf("Curr node: %s\n", token_get_type_literal(curr->expr->tok.type));
        compile_expr(c, curr->expr);

        curr = curr->next;
    }

    emit_byte(c->vm, OP_EXIT);

    return COMPILER_OK;
}
