/**
 * Lexer.hpp
 * =========
 * Phase 1 — Lexical Analysis header for the micro-compiler.
 *
 * Converted from C (lexer.h / lexer.c) to modern, object-oriented C++14.
 *
 * Compatibility note
 * ------------------
 *  Targets C++14 (GCC 6+) to stay compatible with the MinGW toolchain that
 *  ships with older Windows environments.  `std::string_view` (C++17) is
 *  intentionally avoided; `const std::string&` is used instead.
 *
 * Design decisions
 * ----------------
 *  • TokenType is an `enum class` to prevent name-clashes with the global
 *    C standard-library `EOF` macro.  The terminal token is renamed to
 *    `TokenType::END_OF_FILE`.
 *  • Token stores its lexeme in a `std::string` (no fixed-width char arrays,
 *    no strncpy, no buffer-overflow risk).
 *  • Lexer encapsulates all scanner state (source, pos, length) and exposes a
 *    clean public interface — no raw pointers visible to callers.
 *  • `tokenize()` returns `std::vector<Token>` — no manual malloc/realloc/free.
 */

#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// TokenType
// ---------------------------------------------------------------------------

enum class TokenType {
    INT,
    PLUS,
    MINUS,
    MULT,
    DIV,
    END_OF_FILE,   // renamed from TOKEN_EOF to avoid collision with <cstdio> EOF
    ILLEGAL
};

/** Returns the human-readable name of a TokenType (e.g. "PLUS"). */
const char* tokenTypeName(TokenType type) noexcept;

// ---------------------------------------------------------------------------
// Token
// ---------------------------------------------------------------------------

struct Token {
    TokenType   type;
    std::string value;    ///< lexeme (replaces fixed char value[64])
    int         position; ///< byte offset in the source string

    /** Pretty-print to std::cout, matching the original column-aligned format. */
    void print() const;
};

// ---------------------------------------------------------------------------
// Lexer
// ---------------------------------------------------------------------------

class Lexer {
public:
    /**
     * Construct a Lexer over `source`.
     * The Lexer stores a copy of the source string, ensuring it is always
     * valid for the lifetime of the Lexer object.
     */
    explicit Lexer(const std::string& source);

    /** Scan and return the next Token from the source. */
    Token nextToken();

    /**
     * Convenience: tokenize the entire source and return all tokens
     * (including the final END_OF_FILE sentinel) in a vector.
     *
     * Replaces the C functions lexer_tokenize() + lexer_free_tokens().
     * No manual memory management required by the caller.
     */
    static std::vector<Token> tokenize(const std::string& source);

private:
    std::string m_source;  ///< owned copy of the input
    int         m_pos;
    int         m_length;

    // Internal helpers
    char currentChar() const noexcept;
    void advance() noexcept;
    void skipWhitespace() noexcept;
    Token scanNumber();
};

#endif // LEXER_HPP
