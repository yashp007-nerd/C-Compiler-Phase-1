/**
 * test_lexer.c
 * ============
 * Unit test suite for Phase 1 — Lexical Analysis.
 *
 * Each test_ function is self-contained and returns 1 (pass) or 0 (fail).
 * A simple test runner at the bottom executes all tests and prints a
 * summary report.
 *
 * Tests covered
 * -------------
 *  1. Standard addition          "5 + 12"
 *  2. Whitespace handling        "  42   *   2  "
 *  3. Illegal character skipping "10 $ 5"
 *  4. All four operators         "1+2-3*4/5"
 *  5. Multiple spaces/tabs mixed "7\t\t+ \t9"
 *  6. Bare integer               "999"
 *  7. Empty string               ""
 *  8. Only whitespace            "   "
 *  9. Multiple illegal chars     "a + b"
 * 10. Long integer               "123456789 - 987654321"
 */

#include "lexer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ------------------------------------------------------------------ */
/*  Minimal assertion macro                                             */
/* ------------------------------------------------------------------ */

static int g_test_failures = 0;

#define ASSERT_EQ_INT(label, got, expected)                          \
    do {                                                             \
        if ((got) != (expected)) {                                   \
            printf("  [FAIL] %s: expected %d, got %d\n",            \
                   (label), (int)(expected), (int)(got));            \
            g_test_failures++;                                       \
        }                                                            \
    } while (0)

#define ASSERT_EQ_STR(label, got, expected)                          \
    do {                                                             \
        if (strcmp((got), (expected)) != 0) {                        \
            printf("  [FAIL] %s: expected \"%s\", got \"%s\"\n",    \
                   (label), (expected), (got));                      \
            g_test_failures++;                                       \
        }                                                            \
    } while (0)

/* ------------------------------------------------------------------ */
/*  Test helpers                                                        */
/* ------------------------------------------------------------------ */

/** Run lexer_tokenize and assert the token stream matches expectations. */
static int check_tokens(const char *source,
                         const TokenType  *exp_types,
                         const char      **exp_values,
                         const int        *exp_positions,
                         size_t            exp_count)
{
    size_t  count  = 0;
    Token  *tokens = lexer_tokenize(source, &count);
    int     ok     = 1;

    if (!tokens) {
        printf("  [FAIL] lexer_tokenize returned NULL for \"%s\"\n", source);
        return 0;
    }

    if (count != exp_count) {
        printf("  [FAIL] token count: expected %u, got %u  (source: \"%s\")\n",
               (unsigned)exp_count, (unsigned)count, source);
        ok = 0;
    }

    size_t check = count < exp_count ? count : exp_count;
    for (size_t i = 0; i < check; i++) {
        char label[128];

        snprintf(label, sizeof(label), "token[%u].type", (unsigned)i);
        ASSERT_EQ_INT(label, (int)tokens[i].type, (int)exp_types[i]);

        snprintf(label, sizeof(label), "token[%u].value", (unsigned)i);
        ASSERT_EQ_STR(label, tokens[i].value, exp_values[i]);

        if (exp_positions) {
            snprintf(label, sizeof(label), "token[%u].position", (unsigned)i);
            ASSERT_EQ_INT(label, tokens[i].position, exp_positions[i]);
        }
    }

    lexer_free_tokens(tokens);
    return ok;
}

/* ------------------------------------------------------------------ */
/*  Individual tests                                                    */
/* ------------------------------------------------------------------ */

/*
 * Test 1 — Standard addition: "5 + 12"
 * Expected tokens: INT("5",0), PLUS("+",2), INT("12",4), EOF("",6)
 */
static int test_addition(void)
{
    printf("[TEST 1] Standard addition: \"5 + 12\"\n");

    const char      *source       = "5 + 12";
    const TokenType  types[]      = { TOKEN_INT, TOKEN_PLUS, TOKEN_INT, TOKEN_EOF };
    const char      *values[]     = { "5", "+", "12", "" };
    const int        positions[]  = { 0, 2, 4, 6 };

    return check_tokens(source, types, values, positions, 4);
}

/*
 * Test 2 — Spaces handling: "  42   *   2  "
 * Expected tokens: INT("42",2), MULT("*",7), INT("2",11), EOF("",14)
 */
static int test_whitespace(void)
{
    printf("[TEST 2] Whitespace handling: \"  42   *   2  \"\n");

    const char      *source       = "  42   *   2  ";
    const TokenType  types[]      = { TOKEN_INT, TOKEN_MULT, TOKEN_INT, TOKEN_EOF };
    const char      *values[]     = { "42", "*", "2", "" };
    const int        positions[]  = { 2, 7, 11, 14 };

    return check_tokens(source, types, values, positions, 4);
}

/*
 * Test 3 — Illegal character: "10 $ 5"
 * '$' at position 3 is illegal; lexer should skip it and still emit
 * INT("10",0), INT("5",5), EOF("",6).
 * Error message goes to stderr — we capture/suppress nothing here but
 * verify the resulting token stream is correct.
 */
static int test_illegal_char(void)
{
    printf("[TEST 3] Illegal character: \"10 $ 5\"\n");

    const char      *source       = "10 $ 5";
    const TokenType  types[]      = { TOKEN_INT, TOKEN_INT, TOKEN_EOF };
    const char      *values[]     = { "10", "5", "" };
    const int        positions[]  = { 0, 5, 6 };

    return check_tokens(source, types, values, positions, 3);
}

/*
 * Test 4 — All four operators, no spaces: "1+2-3*4/5"
 * Expected: INT,PLUS,INT,MINUS,INT,MULT,INT,DIV,INT,EOF  (10 tokens)
 */
static int test_all_operators(void)
{
    printf("[TEST 4] All operators, no spaces: \"1+2-3*4/5\"\n");

    const char      *source       = "1+2-3*4/5";
    const TokenType  types[]      = {
        TOKEN_INT, TOKEN_PLUS,  TOKEN_INT, TOKEN_MINUS,
        TOKEN_INT, TOKEN_MULT,  TOKEN_INT, TOKEN_DIV,
        TOKEN_INT, TOKEN_EOF
    };
    const char      *values[]     = {
        "1", "+", "2", "-", "3", "*", "4", "/", "5", ""
    };
    const int        positions[]  = { 0,1,2,3,4,5,6,7,8,9 };

    return check_tokens(source, types, values, positions, 10);
}

/*
 * Test 5 — Mixed spaces and tabs: "7\t\t+ \t9"
 */
static int test_mixed_whitespace(void)
{
    printf("[TEST 5] Mixed spaces/tabs: \"7\\t\\t+ \\t9\"\n");

    const char      *source       = "7\t\t+ \t9";
    const TokenType  types[]      = { TOKEN_INT, TOKEN_PLUS, TOKEN_INT, TOKEN_EOF };
    const char      *values[]     = { "7", "+", "9", "" };

    return check_tokens(source, types, values, NULL, 4);
}

/*
 * Test 6 — Bare integer: "999"
 */
static int test_bare_integer(void)
{
    printf("[TEST 6] Bare integer: \"999\"\n");

    const char      *source      = "999";
    const TokenType  types[]     = { TOKEN_INT, TOKEN_EOF };
    const char      *values[]    = { "999", "" };
    const int        positions[] = { 0, 3 };

    return check_tokens(source, types, values, positions, 2);
}

/*
 * Test 7 — Empty string: ""
 * Expected: only EOF at position 0.
 */
static int test_empty_source(void)
{
    printf("[TEST 7] Empty source: \"\"\n");

    const char      *source      = "";
    const TokenType  types[]     = { TOKEN_EOF };
    const char      *values[]    = { "" };
    const int        positions[] = { 0 };

    return check_tokens(source, types, values, positions, 1);
}

/*
 * Test 8 — Only whitespace: "   "
 * Expected: only EOF.
 */
static int test_only_whitespace(void)
{
    printf("[TEST 8] Only whitespace: \"   \"\n");

    const char      *source      = "   ";
    const TokenType  types[]     = { TOKEN_EOF };
    const char      *values[]    = { "" };

    return check_tokens(source, types, values, NULL, 1);
}

/*
 * Test 9 — Multiple illegal chars: "a + b"
 * 'a' (pos 0) and 'b' (pos 4) are illegal; PLUS should still appear.
 * Expected: TOKEN_PLUS("+",2), TOKEN_EOF
 */
static int test_multiple_illegal(void)
{
    printf("[TEST 9] Multiple illegal chars: \"a + b\"\n");

    const char      *source      = "a + b";
    const TokenType  types[]     = { TOKEN_PLUS, TOKEN_EOF };
    const char      *values[]    = { "+", "" };
    const int        positions[] = { 2, 5 };

    return check_tokens(source, types, values, positions, 2);
}

/*
 * Test 10 — Long integers: "123456789 - 987654321"
 */
static int test_long_integers(void)
{
    printf("[TEST 10] Long integers: \"123456789 - 987654321\"\n");

    const char      *source      = "123456789 - 987654321";
    const TokenType  types[]     = { TOKEN_INT, TOKEN_MINUS, TOKEN_INT, TOKEN_EOF };
    const char      *values[]    = { "123456789", "-", "987654321", "" };
    const int        positions[] = { 0, 10, 12, 21 };

    return check_tokens(source, types, values, positions, 4);
}

/* ------------------------------------------------------------------ */
/*  Test runner                                                         */
/* ------------------------------------------------------------------ */

int main(void)
{
    printf("=============================================================\n");
    printf("  Micro-Compiler — Phase 1 Lexer  |  Unit Test Suite\n");
    printf("=============================================================\n\n");

    typedef int (*TestFn)(void);
    TestFn tests[] = {
        test_addition,
        test_whitespace,
        test_illegal_char,
        test_all_operators,
        test_mixed_whitespace,
        test_bare_integer,
        test_empty_source,
        test_only_whitespace,
        test_multiple_illegal,
        test_long_integers
    };

    int total  = (int)(sizeof(tests) / sizeof(tests[0]));
    int passed = 0;

    /* Reset global failure counter before each test. */
    for (int i = 0; i < total; i++) {
        int prev_failures = g_test_failures;
        int result        = tests[i]();

        /* A test is considered passing only if it returned 1 AND
           no new assertion failures were recorded. */
        if (result && g_test_failures == prev_failures) {
            printf("  => PASS\n");
            passed++;
        } else {
            printf("  => FAIL\n");
        }
        printf("\n");
    }

    printf("=============================================================\n");
    printf("  Results: %d / %d tests passed", passed, total);
    if (passed == total)
        printf("  *** ALL TESTS PASSED ***\n");
    else
        printf("  (%d failure(s))\n", total - passed);
    printf("=============================================================\n");

    return (passed == total) ? 0 : 1;
}
