#pragma once

// Minimalne narzedzia do testow - bez zewnetrznych bibliotek.

#include <cmath>
#include <iostream>

inline int& testFailures() {
    static int failures = 0;
    return failures;
}

#define CHECK(cond)                                                                        \
    do {                                                                                   \
        if (!(cond)) {                                                                     \
            std::cerr << __FILE__ << ":" << __LINE__ << ": CHECK nie spelniony: " #cond "\n"; \
            ++testFailures();                                                              \
        }                                                                                  \
    } while (0)

#define CHECK_NEAR(a, b, eps) CHECK(std::abs((a) - (b)) <= (eps))

// Sprawdza, ze wyrazenie rzuca ParseError z podanym numerem wiersza.
#define CHECK_PARSE_ERROR_LINE(expr, expectedLine)                                         \
    do {                                                                                   \
        bool thrown_ = false;                                                              \
        try {                                                                              \
            expr;                                                                          \
        } catch (const ParseError& e_) {                                                   \
            thrown_ = true;                                                                \
            CHECK(e_.line() == (expectedLine));                                            \
        }                                                                                  \
        CHECK(thrown_);                                                                    \
    } while (0)

inline int testResult() {
    if (testFailures() == 0) {
        std::cout << "OK\n";
        return 0;
    }
    std::cerr << testFailures() << " blad(y)\n";
    return 1;
}
