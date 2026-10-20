/**
 * test_lexer.cpp
 * ==============
 * Unit test suite for the micro-compiler Lexer (Phase 1 — C++ edition).
 *
 * No external framework required.  Each test function returns true on pass,
 * false on fail.  A summary is printed at the end.
 *
 * Build:
 *   make test
 *   -- or manually --
 *   g++ -std=c++14 -Wall -Wextra -g -o test_lexer.exe Lexer.cpp test_lexer.cpp
 */

#include "Lexer.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <iomanip>

// ============================================================================
// Minimal test harness
// ============================================================================

static int g_total  = 0;
static int g_passed = 0;

// Registers and immediately runs one named test
static void run_test(const std::string& name, std::function<bool()> fn) {
    ++g_total;
    bool ok = fn();
    if (ok) {
        ++g_passed;
        std::cout << "  [PASS] " << name << "\n";
    } else {
        std::cout << "  [FAIL] " << name << "\n";
    }
}

// Helper: assert a single token field
static bool expect_token(const Token& tok,
                          TokenType   expectedType,
                          const std::string& expectedValue,
                          int         expectedPos)
{
    if (tok.type != expectedType) {
        std::cout << "         type mismatch: got \""
                  << tokenTypeName(tok.type)
                  << "\", expected \""
                  << tokenTypeName(expectedType) << "\"\n";
        return false;
    }
    if (tok.value != expectedValue) {
        std::cout << "         value mismatch: got \""
                  << tok.value
                  << "\", expected \""
                  << expectedValue << "\"\n";
        return false;
    }
    if (tok.position != expectedPos) {
        std::cout << "         position mismatch: got "
                  << tok.position
                  << ", expected "
                  << expectedPos << "\n";
        return false;
    }
    return true;
}

// ============================================================================
// Test cases
// ============================================================================

// --- 1. Empty source --------------------------------------------------------
static bool test_empty_source() {
    auto tokens = Lexer::tokenize("");
    if (tokens.size() != 1) return false;
    return expect_token(tokens[0], TokenType::END_OF_FILE, "", 0);
}

// --- 2. Single integer ------------------------------------------------------
static bool test_single_integer() {
    auto tokens = Lexer::tokenize("42");
    if (tokens.size() != 2) return false;
    return expect_token(tokens[0], TokenType::INT, "42", 0)
        && expect_token(tokens[1], TokenType::END_OF_FILE, "", 2);
}

// --- 3. Single-digit integer ------------------------------------------------
static bool test_single_digit() {
    auto tokens = Lexer::tokenize("7");
    if (tokens.size() != 2) return false;
    return expect_token(tokens[0], TokenType::INT, "7", 0)
        && expect_token(tokens[1], TokenType::END_OF_FILE, "", 1);
}

// --- 4. Multi-digit integer -------------------------------------------------
static bool test_multi_digit_integer() {
    auto tokens = Lexer::tokenize("12345");
    if (tokens.size() != 2) return false;
    return expect_token(tokens[0], TokenType::INT, "12345", 0)
        && expect_token(tokens[1], TokenType::END_OF_FILE, "", 5);
}

// --- 5. Plus operator -------------------------------------------------------
static bool test_plus_operator() {
    auto tokens = Lexer::tokenize("+");
    if (tokens.size() != 2) return false;
    return expect_token(tokens[0], TokenType::PLUS, "+", 0)
        && expect_token(tokens[1], TokenType::END_OF_FILE, "", 1);
}

// --- 6. Minus operator ------------------------------------------------------
static bool test_minus_operator() {
    auto tokens = Lexer::tokenize("-");
    if (tokens.size() != 2) return false;
    return expect_token(tokens[0], TokenType::MINUS, "-", 0)
        && expect_token(tokens[1], TokenType::END_OF_FILE, "", 1);
}

// --- 7. Multiply operator ---------------------------------------------------
static bool test_mult_operator() {
    auto tokens = Lexer::tokenize("*");
    if (tokens.size() != 2) return false;
    return expect_token(tokens[0], TokenType::MULT, "*", 0)
        && expect_token(tokens[1], TokenType::END_OF_FILE, "", 1);
}

// --- 8. Divide operator -----------------------------------------------------
static bool test_div_operator() {
    auto tokens = Lexer::tokenize("/");
    if (tokens.size() != 2) return false;
    return expect_token(tokens[0], TokenType::DIV, "/", 0)
        && expect_token(tokens[1], TokenType::END_OF_FILE, "", 1);
}

// --- 9. Simple addition expression ------------------------------------------
static bool test_simple_addition() {
    auto tokens = Lexer::tokenize("15 + 3");
    if (tokens.size() != 4) return false;
    return expect_token(tokens[0], TokenType::INT,         "15", 0)
        && expect_token(tokens[1], TokenType::PLUS,        "+",  3)
        && expect_token(tokens[2], TokenType::INT,         "3",  5)
        && expect_token(tokens[3], TokenType::END_OF_FILE, "",   6);
}

// --- 10. Subtraction expression ---------------------------------------------
static bool test_subtraction() {
    auto tokens = Lexer::tokenize("100 - 7");
    if (tokens.size() != 4) return false;
    return expect_token(tokens[0], TokenType::INT,         "100", 0)
        && expect_token(tokens[1], TokenType::MINUS,       "-",   4)
        && expect_token(tokens[2], TokenType::INT,         "7",   6)
        && expect_token(tokens[3], TokenType::END_OF_FILE, "",    7);
}

// --- 11. Multiplication expression ------------------------------------------
static bool test_multiplication() {
    auto tokens = Lexer::tokenize("6 * 9");
    if (tokens.size() != 4) return false;
    return expect_token(tokens[0], TokenType::INT,         "6", 0)
        && expect_token(tokens[1], TokenType::MULT,        "*", 2)
        && expect_token(tokens[2], TokenType::INT,         "9", 4)
        && expect_token(tokens[3], TokenType::END_OF_FILE, "", 5);
}

// --- 12. Division expression ------------------------------------------------
static bool test_division() {
    auto tokens = Lexer::tokenize("8 / 4");
    if (tokens.size() != 4) return false;
    return expect_token(tokens[0], TokenType::INT,         "8", 0)
        && expect_token(tokens[1], TokenType::DIV,         "/", 2)
        && expect_token(tokens[2], TokenType::INT,         "4", 4)
        && expect_token(tokens[3], TokenType::END_OF_FILE, "", 5);
}

// --- 13. All four operators in one expression -------------------------------
static bool test_all_operators() {
    // "1+2-3*4/5"  — no spaces
    auto tokens = Lexer::tokenize("1+2-3*4/5");
    if (tokens.size() != 10) return false;
    return expect_token(tokens[0], TokenType::INT,         "1", 0)
        && expect_token(tokens[1], TokenType::PLUS,        "+", 1)
        && expect_token(tokens[2], TokenType::INT,         "2", 2)
        && expect_token(tokens[3], TokenType::MINUS,       "-", 3)
        && expect_token(tokens[4], TokenType::INT,         "3", 4)
        && expect_token(tokens[5], TokenType::MULT,        "*", 5)
        && expect_token(tokens[6], TokenType::INT,         "4", 6)
        && expect_token(tokens[7], TokenType::DIV,         "/", 7)
        && expect_token(tokens[8], TokenType::INT,         "5", 8)
        && expect_token(tokens[9], TokenType::END_OF_FILE, "", 9);
}

// --- 14. Leading whitespace is skipped --------------------------------------
static bool test_leading_whitespace() {
    auto tokens = Lexer::tokenize("   99");
    if (tokens.size() != 2) return false;
    return expect_token(tokens[0], TokenType::INT,         "99", 3)
        && expect_token(tokens[1], TokenType::END_OF_FILE, "",   5);
}

// --- 15. Trailing whitespace is skipped -------------------------------------
static bool test_trailing_whitespace() {
    auto tokens = Lexer::tokenize("5   ");
    if (tokens.size() != 2) return false;
    return expect_token(tokens[0], TokenType::INT,         "5", 0)
        && expect_token(tokens[1], TokenType::END_OF_FILE, "", 4);
}

// --- 16. Multiple spaces between tokens ------------------------------------
static bool test_multiple_spaces() {
    auto tokens = Lexer::tokenize("  42   *   2  ");
    if (tokens.size() != 4) return false;
    return expect_token(tokens[0], TokenType::INT,         "42", 2)
        && expect_token(tokens[1], TokenType::MULT,        "*",  7)
        && expect_token(tokens[2], TokenType::INT,         "2",  11)
        && expect_token(tokens[3], TokenType::END_OF_FILE, "",   14);
}

// --- 17. Tab whitespace is skipped ------------------------------------------
static bool test_tab_whitespace() {
    auto tokens = Lexer::tokenize("\t3\t+\t4");
    if (tokens.size() != 4) return false;
    return expect_token(tokens[0], TokenType::INT,         "3", 1)
        && expect_token(tokens[1], TokenType::PLUS,        "+", 3)
        && expect_token(tokens[2], TokenType::INT,         "4", 5)
        && expect_token(tokens[3], TokenType::END_OF_FILE, "", 6);
}

// --- 18. Whitespace-only source ---------------------------------------------
static bool test_whitespace_only() {
    auto tokens = Lexer::tokenize("     ");
    if (tokens.size() != 1) return false;
    return expect_token(tokens[0], TokenType::END_OF_FILE, "", 5);
}

// --- 19. Illegal character is skipped, next valid token returned -----------
static bool test_illegal_char_skipped() {
    // '$' at position 3 should be skipped; "5" at position 5 should follow
    auto tokens = Lexer::tokenize("10 $ 5");
    // Expected: INT(10), INT(5), END_OF_FILE  ('$' is silently skipped)
    if (tokens.size() != 3) return false;
    return expect_token(tokens[0], TokenType::INT,         "10", 0)
        && expect_token(tokens[1], TokenType::INT,         "5",  5)
        && expect_token(tokens[2], TokenType::END_OF_FILE, "",   6);
}

// --- 20. Multiple illegal characters ----------------------------------------
static bool test_multiple_illegal_chars() {
    auto tokens = Lexer::tokenize("@#!");
    // All three are illegal → only END_OF_FILE
    if (tokens.size() != 1) return false;
    return expect_token(tokens[0], TokenType::END_OF_FILE, "", 3);
}

// --- 21. Illegal char between valid tokens ----------------------------------
static bool test_illegal_between_valid() {
    auto tokens = Lexer::tokenize("3 % 4");
    if (tokens.size() != 3) return false;
    return expect_token(tokens[0], TokenType::INT,         "3", 0)
        && expect_token(tokens[1], TokenType::INT,         "4", 4)
        && expect_token(tokens[2], TokenType::END_OF_FILE, "", 5);
}

// --- 22. Token positions are correct ----------------------------------------
static bool test_token_positions() {
    // "1+22" → INT@0, PLUS@1, INT@2, EOF@4
    auto tokens = Lexer::tokenize("1+22");
    if (tokens.size() != 4) return false;
    return (tokens[0].position == 0)
        && (tokens[1].position == 1)
        && (tokens[2].position == 2)
        && (tokens[3].position == 4);
}

// --- 23. END_OF_FILE token value is empty string ----------------------------
static bool test_eof_value_empty() {
    auto tokens = Lexer::tokenize("1");
    return tokens.back().value.empty();
}

// --- 24. tokenTypeName() returns correct strings ----------------------------
static bool test_token_type_names() {
    return std::string(tokenTypeName(TokenType::INT))         == "INT"
        && std::string(tokenTypeName(TokenType::PLUS))        == "PLUS"
        && std::string(tokenTypeName(TokenType::MINUS))       == "MINUS"
        && std::string(tokenTypeName(TokenType::MULT))        == "MULT"
        && std::string(tokenTypeName(TokenType::DIV))         == "DIV"
        && std::string(tokenTypeName(TokenType::END_OF_FILE)) == "END_OF_FILE"
        && std::string(tokenTypeName(TokenType::ILLEGAL))     == "ILLEGAL";
}

// --- 25. Lexer::nextToken() matches Lexer::tokenize() ----------------------
static bool test_next_token_matches_tokenize() {
    const std::string src = "3+5";
    Lexer lexer(src);

    Token t0 = lexer.nextToken();
    Token t1 = lexer.nextToken();
    Token t2 = lexer.nextToken();
    Token t3 = lexer.nextToken();

    return expect_token(t0, TokenType::INT,         "3", 0)
        && expect_token(t1, TokenType::PLUS,        "+", 1)
        && expect_token(t2, TokenType::INT,         "5", 2)
        && expect_token(t3, TokenType::END_OF_FILE, "", 3);
}

// ============================================================================
// main — run all tests and print summary
// ============================================================================

int main() {
    std::cout << "=============================================================\n";
    std::cout << "  Micro-Compiler — Phase 1 Lexer  |  Unit Tests\n";
    std::cout << "=============================================================\n\n";

    // --- Group 1: Edge cases ---
    std::cout << "[ Edge Cases ]\n";
    run_test("Empty source",              test_empty_source);
    run_test("Single integer",            test_single_integer);
    run_test("Single digit",              test_single_digit);
    run_test("Multi-digit integer",       test_multi_digit_integer);

    // --- Group 2: Operators ---
    std::cout << "\n[ Operators ]\n";
    run_test("Plus operator",             test_plus_operator);
    run_test("Minus operator",            test_minus_operator);
    run_test("Multiply operator",         test_mult_operator);
    run_test("Divide operator",           test_div_operator);

    // --- Group 3: Expressions ---
    std::cout << "\n[ Expressions ]\n";
    run_test("Simple addition",           test_simple_addition);
    run_test("Subtraction",               test_subtraction);
    run_test("Multiplication",            test_multiplication);
    run_test("Division",                  test_division);
    run_test("All four operators",        test_all_operators);

    // --- Group 4: Whitespace ---
    std::cout << "\n[ Whitespace Handling ]\n";
    run_test("Leading whitespace",        test_leading_whitespace);
    run_test("Trailing whitespace",       test_trailing_whitespace);
    run_test("Multiple spaces",           test_multiple_spaces);
    run_test("Tab whitespace",            test_tab_whitespace);
    run_test("Whitespace-only source",    test_whitespace_only);

    // --- Group 5: Illegal characters ---
    std::cout << "\n[ Illegal Characters ]\n";
    run_test("Illegal char skipped",      test_illegal_char_skipped);
    run_test("Multiple illegal chars",    test_multiple_illegal_chars);
    run_test("Illegal between valid",     test_illegal_between_valid);

    // --- Group 6: Correctness ---
    std::cout << "\n[ Correctness ]\n";
    run_test("Token positions",           test_token_positions);
    run_test("EOF value is empty",        test_eof_value_empty);
    run_test("tokenTypeName() strings",   test_token_type_names);
    run_test("nextToken() == tokenize()", test_next_token_matches_tokenize);

    // --- Summary ---
    std::cout << "\n=============================================================\n";
    std::cout << "  Results: " << g_passed << " / " << g_total << " tests passed";
    if (g_passed == g_total)
        std::cout << "  ✓  ALL PASSED\n";
    else
        std::cout << "  ✗  " << (g_total - g_passed) << " FAILED\n";
    std::cout << "=============================================================\n";

    return (g_passed == g_total) ? 0 : 1;
}
