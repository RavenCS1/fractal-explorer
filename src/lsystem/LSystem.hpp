#pragma once

#include <map>
#include <string>

class LineReader;

struct LSystem {
    static constexpr int kMaxIterations = 15;

    std::string axiom;
    std::map<char, std::string> rules;
    double angle = 90.0;
    int iterations = 4;

    static LSystem parse(LineReader& in);
    std::string toText() const;

    bool operator==(const LSystem& other) const {
        return axiom == other.axiom && rules == other.rules && angle == other.angle && iterations == other.iterations;
    }
    bool operator!=(const LSystem& other) const { return !(*this == other); }
};