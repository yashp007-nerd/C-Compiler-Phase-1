/**
 * lexer.c
 * =======
 * Phase 1 — Lexical Analysis implementation for the micro-compiler.
 *
 * Implements the Lexer API declared in lexer.h.  The lexer consumes a
 * null-terminated source string one character at a time and emits a
 * stream of Tokens.
 *
 * Behaviour summary
 * -----------------
 *  - Whitespace (space / tab) is silently skipped.
 *  - Digit sequences are consumed greedily and emitted as TOKEN_INT.
 *  - Single-character operators (+, -, *, /) are mapped to their
 *    respective token types.
 *  - Any other character triggers an error message on stderr (with the
 *    character and its position) and is skipped; scanning continues.
 *  - When the source is exhausted, TOKEN_EOF is returned indefinitely.
 */

#include "lexer.h"

#include <stdio.h>    /* fprintf, printf, snprintf */
#include <stdlib.h>   /* malloc, realloc, free     */
#include <string.h>   /* strlen, strncpy, memset   */
#include <ctype.h>    /* isdigit, isspace          */

/* Human-readable names — kept here (not in the header) to avoid
   -Wunused-variable warnings in translation units that include lexer.h
   but do not call token_print(). */
static const char *TOKEN_TYPE_NAMES[] = {
    "INT",
    "PLUS",
    "MINUS",
    "MULT",
    "DIV",
    "EOF",
    "ILLEGAL"
};

/* ------------------------------------------------------------------ */
/*  Internal helpers                                                    */
/* ------------------------------------------------------------------ */

/** Return the current character without advancing, or '\0' at EOF. */
static char current_char(const Lexer *lexer)
{
    if (lexer->pos >= lexer->length)
        return '\0';
    return lexer->source[lexer->pos];
}

/** Advance the read position by one. */
static void advance(Lexer *lexer)
{
    if (lexer->pos < lexer->length)
        lexer->pos++;
}

/** Skip all whitespace (space and tab) characters. */
static void skip_whitespace(Lexer *lexer)
{
    while (lexer->pos < lexer->length) {
        char c = current_char(lexer);
        if (c == ' ' || c == '\t')
            advance(lexer);
        else
            break;
    }
}

/**
 * make_token — Convenience constructor for a Token.
 *
 * @param type      The token's type.
 * @param value     Null-terminated string value (copied into the struct).
 * @param position  0-based character index in the source.
 */
static Token make_token(TokenType type, const char *value, int position)
{
    Token t;
    t.type     = type;
    t.position = position;
    /* Safe copy — value is always shorter than TOKEN_VALUE_MAX for our uses. */
    strncpy(t.value, value, TOKEN_VALUE_MAX - 1);
    t.value[TOKEN_VALUE_MAX - 1] = '\0';
    return t;
}

/* ------------------------------------------------------------------ */
/*  Public API implementation                                           */
/* ------------------------------------------------------------------ */

void lexer_init(Lexer *lexer, const char *source)
{
    lexer->source = source;
    lexer->pos    = 0;
    lexer->length = (int)strlen(source);
}

/* ---------- lexer_next_token ---------------------------------------- */

Token lexer_next_token(Lexer *lexer)
{
    /* Skip leading whitespace each call — matches "skip and keep scanning"
       semantics required by the spec. */
    skip_whitespace(lexer);

    /* ---- End of source ---- */
    if (lexer->pos >= lexer->length)
        return make_token(TOKEN_EOF, "", lexer->pos);

    char  c   = current_char(lexer);
    int   pos = lexer->pos;

    /* ---- Integer literal ---- */
    if (isdigit((unsigned char)c)) {
        char   buf[TOKEN_VALUE_MAX];
        int    buf_idx = 0;

        while (lexer->pos < lexer->length &&
               isdigit((unsigned char)current_char(lexer))) {
            if (buf_idx < TOKEN_VALUE_MAX - 1)
                buf[buf_idx++] = current_char(lexer);
            advance(lexer);
        }
        buf[buf_idx] = '\0';
        return make_token(TOKEN_INT, buf, pos);
    }

    /* ---- Single-character operators ---- */
    advance(lexer);   /* consume the character before returning */

    switch (c) {
        case '+': return make_token(TOKEN_PLUS,  "+", pos);
        case '-': return make_token(TOKEN_MINUS, "-", pos);
        case '*': return make_token(TOKEN_MULT,  "*", pos);
        case '/': return make_token(TOKEN_DIV,   "/", pos);
        default:
            /*
             * Unknown / illegal character.
             * Log an error and recurse so the caller always gets a
             * meaningful token (not a stale ILLEGAL sentinel).
             */
            fprintf(stderr,
                    "[Lexer Error] Illegal character '%c' (0x%02X) "
                    "at position %d — skipping.\n",
                    c, (unsigned char)c, pos);
            /* Tail-recurse: keep scanning for the next valid token. */
            return lexer_next_token(lexer);
    }
}

/* ---------- token_print -------------------------------------------- */

void token_print(Token t)
{
    printf("Token { type: %-8s, value: \"%-10s\", position: %d }\n",
           TOKEN_TYPE_NAMES[t.type],
           t.value,
           t.position);
}

/* ---------- lexer_tokenize ------------------------------------------ */

Token *lexer_tokenize(const char *source, size_t *out_count)
{
    Lexer lexer;
    lexer_init(&lexer, source);

    size_t  capacity = 16;
    size_t  count    = 0;
    Token  *tokens   = (Token *)malloc(capacity * sizeof(Token));

    if (!tokens) {
        fprintf(stderr, "[Lexer] malloc failed in lexer_tokenize.\n");
        *out_count = 0;
        return NULL;
    }

    for (;;) {
        /* Grow the buffer if needed (double-capacity strategy). */
        if (count >= capacity) {
            capacity *= 2;
            Token *tmp = (Token *)realloc(tokens, capacity * sizeof(Token));
            if (!tmp) {
                fprintf(stderr, "[Lexer] realloc failed in lexer_tokenize.\n");
                free(tokens);
                *out_count = 0;
                return NULL;
            }
            tokens = tmp;
        }

        Token t = lexer_next_token(&lexer);
        tokens[count++] = t;

        if (t.type == TOKEN_EOF)
            break;
    }

    *out_count = count;
    return tokens;
}

/* ---------- lexer_free_tokens --------------------------------------- */

void lexer_free_tokens(Token *tokens)
{
    free(tokens);
}
