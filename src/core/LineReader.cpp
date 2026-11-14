#include "core/LineReader.hpp"

#include <charconv>
#include <cmath>
#include <fstream>
#include <sstream>

namespace {

bool isSpace(char c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\v' || c == '\f';
}

std::string trim(const std::string& s) {
    std::size_t b = 0, e = s.size();
    while (b < e && isSpace(s[b]))
        ++b;
    while (e > b && isSpace(s[e - 1]))
        --e;
    return s.substr(b, e - b);
}

} // namespace

LineReader LineReader::fromFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file)
        throw ParseError(0, "Cannot open file: " + path);
    std::ostringstream ss;
    ss << file.rdbuf();
    return LineReader(ss.str());
}

LineReader LineReader::fromString(std::string text) {
    return LineReader(std::move(text));
}

bool LineReader::next() {
    while (pos_ < text_.size()) {
        std::size_t end = text_.find('\n', pos_);
        if (end == std::string::npos)
            end = text_.size();
        std::string raw = text_.substr(pos_, end - pos_);
        pos_ = end + 1;
        ++lineNumber_;

        const std::size_t hash = raw.find('#');
        if (hash != std::string::npos)
            raw.erase(hash);
        line_ = trim(raw);
        if (!line_.empty())
            return true;
    }
    line_.clear();
    return false;
}

std::vector<double> LineReader::numbers() const {
    std::vector<double> result;
    const char* p = line_.data();
    const char* end = p + line_.size();

    while (p < end) {
        while (p < end && (isSpace(*p) || *p == ','))
            ++p;
        if (p == end)
            break;

        const char* tokenEnd = p;
        while (tokenEnd < end && !isSpace(*tokenEnd) && *tokenEnd != ',')
            ++tokenEnd;
        const std::string token(p, tokenEnd);

        // from_chars does not accept a leading '+' — skip it manually.
        const char* first = (*p == '+') ? p + 1 : p;
        double value = 0.0;
        const auto [ptr, ec] = std::from_chars(first, tokenEnd, value);
        if (ec != std::errc() || ptr != tokenEnd || !std::isfinite(value))
            fail("Invalid number: \"" + token + "\"");

        result.push_back(value);
        p = tokenEnd;
    }
    return result;
}

std::vector<double> LineReader::numbers(std::size_t expected) const {
    std::vector<double> result = numbers();
    if (result.size() != expected)
        fail("Expected " + std::to_string(expected) + " numbers, got " + std::to_string(result.size()));
    return result;
}

int LineReader::integer() const {
    const std::vector<double> v = numbers(1);
    if (v[0] != std::floor(v[0]) || std::abs(v[0]) > 1e9)
        fail("Expected an integer");
    return static_cast<int>(v[0]);
}

void LineReader::fail(const std::string& message) const {
    throw ParseError(lineNumber_, message);
}
