/**
 * main.c
 * ======
 * Demo driver for the micro-compiler lexer (Phase 1).
 *
 * Tokenises several sample expressions and prints each token to stdout.
 * Illegal characters are logged to stderr by the lexer itself.
 */

#include "lexer.h"

#include <stdio.h>
#include <stdlib.h>

static void demo(const char *label, const char *source)
{
    printf("\n--- %s ---\n", label);
    printf("Source: \"%s\"\n\n", source);

    size_t  count  = 0;
    Token  *tokens = lexer_tokenize(source, &count);

    if (!tokens) {
        fprintf(stderr, "Failed to tokenise: \"%s\"\n", source);
        return;
    }

    for (size_t i = 0; i < count; i++)
        token_print(tokens[i]);

    lexer_free_tokens(tokens);
}

int main(void)
{
    printf("=============================================================\n");
    printf("  Micro-Compiler — Phase 1 Lexer  |  Demo\n");
    printf("=============================================================\n");

    demo("Simple addition",      "15 + 3");
    demo("Multiplication",       "  42   *   2  ");
    demo("Illegal character '$'","10 $ 5");
    demo("All operators",        "1+2-3*4/5");
    demo("Empty string",         "");

    printf("\n=============================================================\n");
    return 0;
}
