#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "parser.h"
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
    /* TODO: Add list of arguments output here */
    /* -v or --version, -h or --help */
    printf("Help not implemented\n");
}

static void print_info()
{
    /* TODO: Implement more verbose output here */
    printf("Phantom 0.1\n");
}

static void print_usage()
{
    printf("\nUsage: phantom [options] [arguments...]\n\n");
    printf("To start phantom in interactive mode(REPL)\n  phantom\n\n");
    printf("For help type:\n  phantom -h\n\n");
}

static void run_inline(lexer_t *l, parser_t *p, vm_t *vm)
{
#ifdef LEXER_DEBUG_OUTPUT
    debug_print_token_header();

    token_t tok;
    while ( (tok = lexer_next(l)).type != TOK_EOF)
    {
        debug_print_token(tok);
    }
#endif

#ifdef PARSER_DEBUG_OUTPUT
    parser_parse(p);
#endif

    vm_run(vm);
}

static void repl()
{
    char input[1024];

    vm_t *vm = vm_init();
    lexer_t *l = NULL;//lexer_init(input);
    parser_t *p = NULL;//parser_init(input, vm);

    for (;;)
    {
        printf(">> ");

        if (!fgets(input, sizeof(input), stdin))
        {
            printf("\n");
            break;
        }

        l = lexer_init(input);
        p = parser_init(input, vm);

        run_inline(l, p, vm);


        parser_free(p);
        lexer_free(l);
    }

    vm_free(vm);
}


static void check_args(int argc, char **argv)
{
    if (argc == 1)
    {
        repl();
        //print_usage();
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
    }
}

int main(int argc, char **argv)
{
    check_args(argc, argv);


    //char *input = read_file(argv[1]);

    //free(input);

    return 0;
}


