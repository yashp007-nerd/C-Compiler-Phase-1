/**
 * main.cpp
 * ========
 * Demo driver for the micro-compiler Lexer (Phase 1) — C++ edition.
 *
 * Tokenises several sample expressions and prints each token to std::cout.
 * Illegal characters are reported to std::cerr by the Lexer itself.
 *
 * Build:
 *   make           (uses the project Makefile)
 *   -- or manually --
 *   g++ -std=c++14 -Wall -Wextra -g -o main_cpp.exe Lexer.cpp main.cpp
 */

#include "Lexer.hpp"

#include <iostream>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// demo — tokenise one source string and print the result
// ---------------------------------------------------------------------------

static void demo(const std::string& label, const std::string& source) {
    std::cout << "\n--- " << label << " ---\n";
    std::cout << "Source: \"" << source << "\"\n\n";

    std::vector<Token> tokens = Lexer::tokenize(source);
    for (const Token& tok : tokens)
        tok.print();
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------

int main() {
    std::cout << "=============================================================\n";
    std::cout << "  Micro-Compiler — Phase 1 Lexer  |  C++ Demo\n";
    std::cout << "=============================================================\n";

    demo("Simple addition",       "15 + 3");
    demo("Multiplication",        "  42   *   2  ");
    demo("Illegal character '$'", "10 $ 5");
    demo("All operators",         "1+2-3*4/5");
    demo("Empty string",          "");

    std::cout << "\n=============================================================\n";
    return 0;
}
