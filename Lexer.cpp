/**
 * Lexer.cpp
 * =========
 * Phase 1 — Lexical Analysis implementation for the micro-compiler.
 *
 * Converted from C (lexer.c) to modern, object-oriented C++14.
 */

#include "Lexer.hpp"

#include <cctype>    // std::isdigit
#include <iomanip>   // std::setw, std::left
#include <iostream>  // std::cout, std::cerr
#include <cstdio>    // snprintf (for hex formatting on GCC 6)

// ---------------------------------------------------------------------------
// tokenTypeName — maps enum class values to C-string literals
// ---------------------------------------------------------------------------

const char* tokenTypeName(TokenType type) noexcept {
    switch (type) {
        case TokenType::INT:         return "INT";
        case TokenType::PLUS:        return "PLUS";
        case TokenType::MINUS:       return "MINUS";
        case TokenType::MULT:        return "MULT";
        case TokenType::DIV:         return "DIV";
        case TokenType::END_OF_FILE: return "END_OF_FILE";
        case TokenType::ILLEGAL:     return "ILLEGAL";
    }
    return "UNKNOWN"; // unreachable, silences -Wreturn-type
}

// ---------------------------------------------------------------------------
// Token::print
// ---------------------------------------------------------------------------

void Token::print() const {
    // Preserve the original column-aligned format:
    //   Token { type: INT         , value: "15"       , position: 0 }
    std::cout << "Token { type: "
              << std::left << std::setw(12) << tokenTypeName(type)
              << ", value: \""
              << std::left << std::setw(10) << value
              << "\", position: "
              << position
              << " }\n";
}

// ---------------------------------------------------------------------------
// Lexer — constructor
// ---------------------------------------------------------------------------

Lexer::Lexer(const std::string& source)
    : m_source(source)
    , m_pos(0)
    , m_length(static_cast<int>(source.size()))
{}

// ---------------------------------------------------------------------------
// Lexer — private helpers
// ---------------------------------------------------------------------------

char Lexer::currentChar() const noexcept {
    if (m_pos >= m_length) return '\0';
    return m_source[static_cast<std::size_t>(m_pos)];
}

void Lexer::advance() noexcept {
    if (m_pos < m_length) ++m_pos;
}

void Lexer::skipWhitespace() noexcept {
    while (m_pos < m_length) {
        char c = currentChar();
        if (c == ' ' || c == '\t')
            advance();
        else
            break;
    }
}

Token Lexer::scanNumber() {
    int startPos = m_pos;
    std::string lexeme;
    while (m_pos < m_length &&
           std::isdigit(static_cast<unsigned char>(currentChar()))) {
        lexeme += currentChar();
        advance();
    }
    return Token{ TokenType::INT, std::move(lexeme), startPos };
}

// ---------------------------------------------------------------------------
// Lexer::nextToken
// ---------------------------------------------------------------------------

Token Lexer::nextToken() {
    skipWhitespace();

    if (m_pos >= m_length)
        return Token{ TokenType::END_OF_FILE, "", m_pos };

    char c   = currentChar();
    int  pos = m_pos;

    // Integer literal
    if (std::isdigit(static_cast<unsigned char>(c)))
        return scanNumber();

    // Single-character operators
    advance();
    switch (c) {
        case '+': return Token{ TokenType::PLUS,  "+", pos };
        case '-': return Token{ TokenType::MINUS, "-", pos };
        case '*': return Token{ TokenType::MULT,  "*", pos };
        case '/': return Token{ TokenType::DIV,   "/", pos };
        default: {
            // Log the illegal character to stderr (preserving the original message)
            char hexBuf[8];
            std::snprintf(hexBuf, sizeof(hexBuf), "%02X",
                          static_cast<unsigned int>(static_cast<unsigned char>(c)));

            std::cerr << "[Lexer Error] Illegal character '"
                      << c
                      << "' (0x" << hexBuf << ")"
                      << " at position " << pos
                      << " — skipping.\n";

            return nextToken(); // tail-recurse: skip and fetch next valid token
        }
    }
}

// ---------------------------------------------------------------------------
// Lexer::tokenize  (static convenience wrapper)
// ---------------------------------------------------------------------------

std::vector<Token> Lexer::tokenize(const std::string& source) {
    Lexer lexer(source);
    std::vector<Token> tokens;
    tokens.reserve(32); // reasonable starting capacity; vector grows as needed

    for (;;) {
        Token t = lexer.nextToken();
        tokens.push_back(t);
        if (t.type == TokenType::END_OF_FILE) break;
    }
    return tokens; // NRVO / move — zero heap leaks possible
}
