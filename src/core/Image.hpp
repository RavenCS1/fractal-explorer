#pragma once

#include "core/Color.hpp"

#include <cstdint>
#include <vector>

// Bufor pikseli, do ktorego rysuja wszystkie renderery.
// Nie zalezy od SFML - mozna go uzywac w testach bez okna.
// Uklad: wiersz po wierszu, (0,0) to lewy gorny rog.
class Image {
public:
    Image() = default;
    Image(int width, int height, Color fill = Color(0, 0, 0));

    // Zmienia rozmiar; dotychczasowa zawartosc jest tracona.
    void resize(int width, int height, Color fill = Color(0, 0, 0));
    void fill(Color c);

    int width() const { return width_; }
    int height() const { return height_; }
    bool empty() const { return pixels_.empty(); }
    bool inside(int x, int y) const { return x >= 0 && y >= 0 && x < width_ && y < height_; }

    // Bezpieczne: piksele poza obrazem sa ignorowane.
    void set(int x, int y, Color c) {
        if (inside(x, y)) pixels_[static_cast<std::size_t>(y) * width_ + x] = c;
    }
    // Poza obrazem zwraca czarny.
    Color get(int x, int y) const {
        return inside(x, y) ? pixels_[static_cast<std::size_t>(y) * width_ + x] : Color(0, 0, 0);
    }

    // Odcinek algorytmem Bresenhama, oba konce wlacznie; fragmenty poza obrazem sa pomijane.
    void drawLine(int x0, int y0, int x1, int y1, Color c);

    // Bezposredni dostep - do szybkich petli i do przekazania do SFML.
    Color* pixels() { return pixels_.data(); }
    const Color* pixels() const { return pixels_.data(); }
    const std::uint8_t* bytes() const { return reinterpret_cast<const std::uint8_t*>(pixels_.data()); }

private:
    int width_ = 0;
    int height_ = 0;
    std::vector<Color> pixels_;
};
