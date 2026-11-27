#pragma once

/**
 * @file
 * L-system definition, parser and string rewriting.
 */

#include <cstddef>
#include <map>
#include <string>

class LineReader;

/**
 * An L-system: an axiom (start string), rewriting rules, the turtle turning angle and an iteration count.
 *
 * Owner: Patryk.
 */
struct LSystem {
    static constexpr int kMaxIterations = 15;            ///< Largest accepted "iterations" value.
    static constexpr std::size_t kMaxLength = 5'000'000; ///< Default limit on the expanded string length.

    /// Result of expand().
    struct Expansion {
        std::string text;          ///< The rewritten string.
        int iterationsDone = 0;    ///< How many iterations were actually applied.
        bool limitReached = false; ///< True when expansion stopped early because of the length limit.
    };

    /**
     * Applies the rules @ref iterations times, replacing all symbols simultaneously.
     * Stops early (and sets Expansion::limitReached) if the next step would exceed @p maxLength symbols.
     */
    [[nodiscard]] Expansion expand(std::size_t maxLength = kMaxLength) const;

    std::string axiom;                 ///< Start string, e.g. "F".
    std::map<char, std::string> rules; ///< Symbol -> replacement, e.g. 'F' -> "F+F-F-F+F".
    double angle = 90.0;               ///< Turtle turning angle in degrees for '+' and '-'.
    int iterations = 4;                ///< Number of rewriting steps.

    /**
     * Parses a .lsys file (keywords axiom, rule, angle, iterations; any order).
     * @throws ParseError with the line number of the offending line, or line 0 for a missing keyword.
     */
    static LSystem parse(LineReader& in);
    /// Serializes back to .lsys format. @todo Not implemented yet (declared only).
    [[nodiscard]] std::string toText() const;

    bool operator==(const LSystem& other) const {
        return axiom == other.axiom && rules == other.rules && angle == other.angle && iterations == other.iterations;
    }
    bool operator!=(const LSystem& other) const { return !(*this == other); }
};
