#pragma once

#include <stdexcept>
#include <string>

// Blad wczytywania pliku definicji (.ifs, .lsys, ...).
// line() to numer wiersza w pliku liczony od 1; 0 oznacza blad calego pliku
// (np. nie da sie go otworzyc albo brakuje sekcji).
class ParseError : public std::runtime_error {
public:
    ParseError(int line, const std::string& message)
        : std::runtime_error(line > 0 ? "Wiersz " + std::to_string(line) + ": " + message : message),
          line_(line) {}

    int line() const { return line_; }

private:
    int line_;
};
