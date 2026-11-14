#pragma once

#include <cstdint>

// RGBA color, 4 bytes in r, g, b, a order — the same layout expected by
// sf::Texture::update() and sf::Image, so the Image buffer is passed without copying.
struct Color {
    std::uint8_t r = 0;
    std::uint8_t g = 0;
    std::uint8_t b = 0;
    std::uint8_t a = 255;

    constexpr Color() = default;
    constexpr Color(std::uint8_t r_, std::uint8_t g_, std::uint8_t b_, std::uint8_t a_ = 255)
        : r(r_), g(g_), b(b_), a(a_) {}

    friend constexpr bool operator==(Color x, Color y) { return x.r == y.r && x.g == y.g && x.b == y.b && x.a == y.a; }
    friend constexpr bool operator!=(Color x, Color y) { return !(x == y); }
};

static_assert(sizeof(Color) == 4, "Color must be exactly 4 bytes (RGBA)");
