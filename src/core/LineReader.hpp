#pragma once

#include "core/ParseError.hpp"

#include <cstddef>
#include <string>
#include <vector>

// Shared text-file reader for all parsers.
//  - skips blank lines and comments (from '#' to end of line),
//  - tracks the current line number so errors carry a line number,
//  - numbers may be separated by spaces, tabs, or commas.
//
// Example:
//     LineReader in = LineReader::fromFile(path);
//     if (!in.next()) in.fail("Missing number of transforms");
//     int n = in.integer();
//     for (int i = 0; i < n; ++i) {
//         if (!in.next()) in.fail("Too few transforms");
//         std::vector<double> v = in.numbers(7);   // a b c d e f p
//     }
class LineReader {
public:
    // Throws ParseError(0, ...) when the file cannot be opened.
    static LineReader fromFile(const std::string& path);
    // For tests: file contents provided directly.
    static LineReader fromString(std::string text);

    // Advances to the next non-empty line. Returns false at end of file.
    bool next();

    // Current line with comment and surrounding whitespace stripped.
    const std::string& line() const { return line_; }
    int lineNumber() const { return lineNumber_; }

    // All numbers from the current line.
    std::vector<double> numbers() const;
    // Exactly 'expected' numbers, otherwise ParseError with line number.
    std::vector<double> numbers(std::size_t expected) const;
    // A single integer on the line.
    int integer() const;

    // Throws ParseError with the current line number (0 if nothing has been read yet).
    [[noreturn]] void fail(const std::string& message) const;

private:
    explicit LineReader(std::string text) : text_(std::move(text)) {}

    std::string text_;
    std::size_t pos_ = 0;
    std::string line_;
    int lineNumber_ = 0;
};
