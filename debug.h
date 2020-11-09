#ifndef __PHANTOM_DEBUG_H_
#define __PHANTOM_DEBUG_H_

#undef LEXER_DEBUG_OUTPUT
#define PARSER_DEBUG_OUTPUT
#define COMPILER_DEBUG_OUTPUT

#include "lexer.h"

void debug_print_token_header(void);
void debug_print_token(token_t tok);

#endif // __PHANTOM_DEBUG_H_
