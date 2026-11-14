#pragma once

#include "core/Color.hpp"

#include <cstdint>
#include <vector>

// Pixel buffer that all renderers draw into.
// Independent of SFML — can be used in headless tests.
// Layout: row by row, (0,0) is the top-left corner.
class Image {
public:
    Image() = default;
    Image(int width, int height, Color fill = Color(0, 0, 0));

    // Resizes the image; existing contents are discarded.
    void resize(int width, int height, Color fill = Color(0, 0, 0));
    void fill(Color c);

    int width() const { return width_; }
    int height() const { return height_; }
    bool empty() const { return pixels_.empty(); }
    bool inside(int x, int y) const { return x >= 0 && y >= 0 && x < width_ && y < height_; }

    // Safe: pixels outside the image are ignored.
    void set(int x, int y, Color c) {
        if (inside(x, y))
            pixels_[static_cast<std::size_t>(y) * width_ + x] = c;
    }
    // Returns black for pixels outside the image.
    Color get(int x, int y) const {
        return inside(x, y) ? pixels_[static_cast<std::size_t>(y) * width_ + x] : Color(0, 0, 0);
    }

    // Bresenham line, both endpoints inclusive; segments outside the image are clipped.
    void drawLine(int x0, int y0, int x1, int y1, Color c);

    // Direct access — for tight loops and passing to SFML.
    Color* pixels() { return pixels_.data(); }
    const Color* pixels() const { return pixels_.data(); }
    const std::uint8_t* bytes() const { return reinterpret_cast<const std::uint8_t*>(pixels_.data()); }

private:
    int width_ = 0;
    int height_ = 0;
    std::vector<Color> pixels_;
};
