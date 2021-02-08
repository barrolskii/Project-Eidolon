#define _CRTDBG_MAP_ALLOC
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <crtdbg.h>
#include "lexer.h"
#include "parser.h"
#include "compiler.h"
#include "vm.h"
#include "debug.h"

static char *read_file(const char *path)
{
    FILE *file = fopen(path, "rb");

    if (!file)
    {
        fprintf(stderr, "Unable to open file '%s'\n", path);
        exit(74); /* IO error */
    }

    fseek(file, 0, SEEK_END);
    size_t fsize = ftell(file);
    rewind(file);

    char *buffer = malloc(fsize + 1);
    size_t bytes = fread(buffer, sizeof(char), fsize, file);

    if (bytes < fsize)
    {
        fprintf(stderr, "Unable to read file '%s'\n", path);
        exit(74);
    }

    buffer[bytes] = '\0';
    fclose(file);

    return buffer;
}

static void print_help()
{
    printf("\nUsage: phantom [options] [arguments...]\n\n");
    printf("To start phantom in interactive mode(REPL)\n  phantom\n\n");
    printf("For help type:\n  phantom -h\n\n");

    /* TODO: Add list of arguments output here */
    /* -v or --version, -h or --help */
    printf("Help not implemented\n");
}

static void print_info()
{
    /* TODO: Implement more verbose output here */
    printf("Phantom 0.1\n");
}

static void run_inline(lexer_t *l)
{
    token_t tok;
    while ( (tok = lexer_next(l)).type != TOK_EOF)
    {
        debug_print_token(tok);
    }
}

static void repl()
{
    char input[1024];

    lexer_t *l = NULL;

    for (;;)
    {
        printf(">> ");

        if (!fgets(input, sizeof(input), stdin))
        {
            printf("\n");
            break;
        }

        l = lexer_init(input);

        run_inline(l);


        free(l);
    }
}

static void check_args(int argc, char **argv)
{
    if (argc == 1)
    {
        repl();
        exit(0);
    }

    if (argc == 2)
    {
        if (strcmp(argv[1], "-h") == 0)
        {
            print_help();
            exit(0);
        }

        if (strcmp(argv[1], "-v") == 0)
        {
            print_info();
            exit(0);
        }

        printf("arg: %s\n", argv[1]);
    }
}

int main(int argc, char **argv)
{
    check_args(argc, argv);

    char *input = read_file(argv[1]);

    lexer_t *l = lexer_init(input);
    parser_t *p = parser_init(l);
    vm_t *vm = vm_init();
    compiler_t *c = compiler_init(vm);

    ast_node_t *ast = parser_parse_program(p);
    //if (!ast) printf("No ast supplied\n");

    compiler_code_t code = compiler_compile_program(c, ast);
    if (code == COMPILER_OK) printf("Successful compilation!\n");

    vm_run(vm);

    //ast_node_print_header();
    //ast_node_print_node(ast);

    ast_node_free(ast);

    vm_free(vm);
    compiler_free(c);
    parser_free(p);

    free(input);

    _CrtDumpMemoryLeaks();
    return 0;
}
