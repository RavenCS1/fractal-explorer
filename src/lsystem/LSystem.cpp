#include "LSystem.hpp"

#include "core/LineReader.hpp"
#include "core/ParseError.hpp"

#include <charconv>
#include <cmath>

namespace {

bool isSpace(char c) {
    return c == ' ' || c == '\t';
}

std::string trim(const std::string& s) {
    std::size_t b = 0, e = s.size();
    while (b < e && isSpace(s[b]))
        ++b;
    while (e > b && isSpace(s[e - 1]))
        --e;
    return s.substr(b, e - b);
}

std::string removeSpaces(const std::string& s) {
    std::string out;
    for (char c : s)
        if (!isSpace(c))
            out += c;
    return out;
}

// Parses the whole string as a real number, e.g. "90" or "22.5".
double parseNumber(const LineReader& in, const std::string& s) {
    const char* first = s.data();
    const char* last = s.data() + s.size();
    if (first != last && *first == '+') // from_chars does not accept a leading '+'
        ++first;

    double value = 0.0;
    const auto [ptr, ec] = std::from_chars(first, last, value);
    if (s.empty() || ec != std::errc() || ptr != last || !std::isfinite(value))
        in.fail("Invalid number: \"" + s + "\"");
    return value;
}

// Parses the whole string as an integer, e.g. "4" (but not "4.5").
int parseInteger(const LineReader& in, const std::string& s) {
    const char* first = s.data();
    const char* last = s.data() + s.size();
    if (first != last && *first == '+')
        ++first;

    int value = 0;
    const auto [ptr, ec] = std::from_chars(first, last, value);
    if (s.empty() || ec != std::errc() || ptr != last)
        in.fail("Expected an integer: \"" + s + "\"");
    return value;
}

} // namespace

LSystem LSystem::parse(LineReader& in) {
    LSystem result;
    bool hasAxiom = false, hasAngle = false, hasIterations = false;

    while (in.next()) {
        const std::string& line = in.line(); // e.g. "rule F=F+F-F-F+F"
        const std::size_t space = line.find_first_of(" \t");
        std::string key = line.substr(0, space);                                              // "rule"
        std::string value = (space == std::string::npos) ? "" : trim(line.substr(space + 1)); // "F=F+F-F-F+F"

        if (key == "axiom") {
            if (hasAxiom)
                in.fail("Duplicate 'axiom'");
            value = removeSpaces(value); // "F + F" -> "F+F"
            if (value.empty())
                in.fail("Empty axiom");
            result.axiom = value;
            hasAxiom = true;
        } else if (key == "rule") {
            std::size_t eq = value.find('=');
            if (eq == std::string::npos)
                in.fail("Expected rule in form X=...");
            std::string lhs = trim(value.substr(0, eq));          // "F"
            std::string rhs = removeSpaces(value.substr(eq + 1)); // "F+F-F-F+F" (may be empty!)
            if (lhs.size() != 1)
                in.fail("Rule must replace exactly one symbol");
            if (!result.rules.emplace(lhs[0], rhs).second)
                in.fail(std::string("Duplicate rule for '") + lhs[0] + "'");
        } else if (key == "angle") {
            if (hasAngle)
                in.fail("Duplicate 'angle'");
            result.angle = parseNumber(in, value); // from_chars + in.fail on error
            hasAngle = true;
        } else if (key == "iterations") {
            if (hasIterations)
                in.fail("Duplicate 'iterations'");
            int n = parseInteger(in, value);
            if (n < 0 || n > kMaxIterations)
                in.fail("Iterations must be between 0 and " + std::to_string(kMaxIterations));
            result.iterations = n;
            hasIterations = true;
        } else {
            in.fail("Unknown keyword '" + key + "'");
        }
    }

    if (!hasAxiom)
        throw ParseError(0, "Missing 'axiom'");
    if (!hasAngle)
        throw ParseError(0, "Missing 'angle'");
    if (!hasIterations)
        throw ParseError(0, "Missing 'iterations'");
    return result;
}