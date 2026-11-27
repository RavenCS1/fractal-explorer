#pragma once

#include <cstddef>
#include <map>
#include <string>

class LineReader;

struct LSystem {
    static constexpr int kMaxIterations = 15;

    static constexpr std::size_t kMaxLength = 5'000'000;

    struct Expansion {
        std::string text;
        int iterationsDone = 0;
        bool limitReached = false;
    };

    [[nodiscard]] Expansion expand(std::size_t maxLength = kMaxLength) const;

    std::string axiom;
    std::map<char, std::string> rules;
    double angle = 90.0;
    int iterations = 4;

    static LSystem parse(LineReader& in);
    [[nodiscard]] std::string toText() const;

    bool operator==(const LSystem& other) const {
        return axiom == other.axiom && rules == other.rules && angle == other.angle && iterations == other.iterations;
    }
    bool operator!=(const LSystem& other) const { return !(*this == other); }
};
