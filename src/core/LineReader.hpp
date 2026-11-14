#pragma once

#include "core/ParseError.hpp"

#include <cstddef>
#include <string>
#include <vector>

// Wspolny czytnik plikow tekstowych dla wszystkich parserow.
//  - pomija puste wiersze i komentarze (od '#' do konca wiersza),
//  - pamieta numer biezacego wiersza, zeby bledy mialy numer wiersza,
//  - liczby moga byc rozdzielone spacjami, tabulatorami lub przecinkami.
//
// Przyklad:
//     LineReader in = LineReader::fromFile(path);
//     if (!in.next()) in.fail("Brak liczby przeksztalcen");
//     int n = in.integer();
//     for (int i = 0; i < n; ++i) {
//         if (!in.next()) in.fail("Za malo przeksztalcen");
//         std::vector<double> v = in.numbers(7);   // a b c d e f p
//     }
class LineReader {
public:
    // Rzuca ParseError(0, ...), gdy pliku nie da sie otworzyc.
    static LineReader fromFile(const std::string& path);
    // Do testow: tresc pliku podana wprost.
    static LineReader fromString(std::string text);

    // Przechodzi do nastepnego niepustego wiersza. Zwraca false na koncu pliku.
    bool next();

    // Biezacy wiersz bez komentarza i bez bialych znakow na brzegach.
    const std::string& line() const { return line_; }
    int lineNumber() const { return lineNumber_; }

    // Wszystkie liczby z biezacego wiersza.
    std::vector<double> numbers() const;
    // Dokladnie 'expected' liczb, inaczej ParseError z numerem wiersza.
    std::vector<double> numbers(std::size_t expected) const;
    // Jedna liczba calkowita w wierszu.
    int integer() const;

    // Rzuca ParseError z numerem biezacego wiersza (0, gdy nic jeszcze nie wczytano).
    [[noreturn]] void fail(const std::string& message) const;

private:
    explicit LineReader(std::string text) : text_(std::move(text)) {}

    std::string text_;
    std::size_t pos_ = 0;
    std::string line_;
    int lineNumber_ = 0;
};
