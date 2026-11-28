#pragma once

/**
 * @file
 * Exception for errors in definition files.
 */

#include <stdexcept>
#include <string>

/**
 * Error while reading a definition file (.ifs, .lsys, ...).
 *
 * line() is the 1-based line number in the file; 0 means a file-level error
 * (e.g. cannot open or a missing section). what() already contains "Line N: ".
 */
class ParseError : public std::runtime_error {
public:
    ParseError(int line, const std::string& message)
        : std::runtime_error(line > 0 ? "Line " + std::to_string(line) + ": " + message : message), line_(line) {}

    /// 1-based line number of the error, 0 for file-level errors.
    [[nodiscard]] int line() const { return line_; }

private:
    int line_;
};
