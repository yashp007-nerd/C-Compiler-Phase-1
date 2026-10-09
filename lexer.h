/**
 * lexer.h
 * =======
 * Phase 1 — Lexical Analysis header for the micro-compiler.
 *
 * Defines the Token data structure, TokenType enum, and the public
 * API of the lexer (Lexer struct + functions).
 */

#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>   /* size_t */

/* ------------------------------------------------------------------ */
/*  Token Types                                                         */
/* ------------------------------------------------------------------ */

typedef enum {
    TOKEN_INT,      /* A positive integer literal, e.g. "42"          */
    TOKEN_PLUS,     /* '+'                                              */
    TOKEN_MINUS,    /* '-'                                              */
    TOKEN_MULT,     /* '*'                                              */
    TOKEN_DIV,      /* '/'                                              */
    TOKEN_EOF,      /* End-of-input sentinel                           */
    TOKEN_ILLEGAL   /* Emitted internally when an invalid char is seen */
} TokenType;

/* ------------------------------------------------------------------ */
/*  Token Structure                                                     */
/* ------------------------------------------------------------------ */

/*
 * Maximum length of a token's value string.
 * A 32-bit integer has at most 10 digits; 64 bytes is more than enough.
 */
#define TOKEN_VALUE_MAX 64

typedef struct {
    TokenType   type;                   /* Categorical label             */
    char        value[TOKEN_VALUE_MAX]; /* Literal text of the token     */
    int         position;              /* 0-based char index in source  */
} Token;

/* ------------------------------------------------------------------ */
/*  Lexer Structure                                                     */
/* ------------------------------------------------------------------ */

typedef struct {
    const char *source;   /* Pointer to the null-terminated input string */
    int         pos;      /* Current read position (0-based)             */
    int         length;   /* Total length of the source string           */
} Lexer;

/* ------------------------------------------------------------------ */
/*  Public API                                                          */
/* ------------------------------------------------------------------ */

/**
 * lexer_init — Initialise a Lexer for the given source string.
 *
 * @param lexer   Pointer to the Lexer to initialise.
 * @param source  Null-terminated input string to tokenise.
 */
void lexer_init(Lexer *lexer, const char *source);

/**
 * lexer_next_token — Advance the lexer and return the next Token.
 *
 * Whitespace (spaces, tabs) is silently skipped.
 * Unknown characters produce an error message on stderr and are skipped;
 * the lexer then continues scanning — no token is emitted for them.
 *
 * @param lexer  Pointer to a previously initialised Lexer.
 * @return       The next Token (ends with TOKEN_EOF when exhausted).
 */
Token lexer_next_token(Lexer *lexer);

/**
 * token_print — Print a human-readable representation of a Token to stdout.
 *
 * Format: Token { type: <TYPE>, value: "<value>", position: <pos> }
 *
 * @param t  The Token to print.
 */
void token_print(Token t);

/**
 * lexer_tokenize — Tokenise the entire source string into a heap-allocated
 *                  array of Tokens terminated by a TOKEN_EOF entry.
 *
 * The caller is responsible for calling lexer_free_tokens() on the result.
 *
 * @param source      Null-terminated input string.
 * @param out_count   Set to the number of tokens in the returned array
 *                    (including the final EOF token).
 * @return            Heap-allocated Token array, or NULL on allocation failure.
 */
Token *lexer_tokenize(const char *source, size_t *out_count);

/**
 * lexer_free_tokens — Free a Token array returned by lexer_tokenize().
 *
 * @param tokens  Pointer returned by lexer_tokenize().
 */
void lexer_free_tokens(Token *tokens);

#endif /* LEXER_H */
