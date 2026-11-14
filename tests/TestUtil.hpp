#pragma once

// Minimal test utilities — no external libraries.

#include <cmath>
#include <iostream>

inline int& testFailures() {
    static int failures = 0;
    return failures;
}

#define CHECK(cond)                                                                                                    \
    do {                                                                                                               \
        if (!(cond)) {                                                                                                 \
            std::cerr << __FILE__ << ":" << __LINE__ << ": CHECK failed: " #cond "\n";                                 \
            ++testFailures();                                                                                          \
        }                                                                                                              \
    } while (0)

#define CHECK_NEAR(a, b, eps) CHECK(std::abs((a) - (b)) <= (eps))

// Checks that the expression throws ParseError with the given line number.
#define CHECK_PARSE_ERROR_LINE(expr, expectedLine)                                                                     \
    do {                                                                                                               \
        bool thrown_ = false;                                                                                          \
        try {                                                                                                          \
            expr;                                                                                                      \
        } catch (const ParseError& e_) {                                                                               \
            thrown_ = true;                                                                                            \
            CHECK(e_.line() == (expectedLine));                                                                        \
        }                                                                                                              \
        CHECK(thrown_);                                                                                                \
    } while (0)

inline int testResult() {
    if (testFailures() == 0) {
        std::cout << "OK\n";
        return 0;
    }
    std::cerr << testFailures() << " failure(s)\n";
    return 1;
}
