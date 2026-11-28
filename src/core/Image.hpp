#pragma once

/**
 * @file
 * The pixel buffer all renderers draw into.
 */

#include "core/Color.hpp"

#include <cstdint>
#include <vector>

/**
 * Pixel buffer that all renderers draw into.
 *
 * Independent of SFML, so it can be used in headless tests.
 * Layout: row by row, (0,0) is the top-left corner.
 */
class Image {
public:
    Image() = default;
    /// Creates a @p width x @p height image filled with @p fill.
    Image(int width, int height, Color fill = Color(0, 0, 0));

    /// Resizes the image; existing contents are discarded.
    void resize(int width, int height, Color fill = Color(0, 0, 0));
    /// Sets every pixel to @p c.
    void fill(Color c);

    [[nodiscard]] int width() const { return width_; }
    [[nodiscard]] int height() const { return height_; }
    /// True when the image has no pixels.
    [[nodiscard]] bool empty() const { return pixels_.empty(); }
    /// True when (@p x, @p y) is a valid pixel.
    [[nodiscard]] bool inside(int x, int y) const { return x >= 0 && y >= 0 && x < width_ && y < height_; }

    /// Sets one pixel. Safe: pixels outside the image are ignored.
    void set(int x, int y, Color c) {
        if (inside(x, y))
            pixels_[(static_cast<std::size_t>(y) * width_) + x] = c;
    }
    /// Reads one pixel. Returns black for pixels outside the image.
    [[nodiscard]] Color get(int x, int y) const {
        return inside(x, y) ? pixels_[(static_cast<std::size_t>(y) * width_) + x] : Color(0, 0, 0);
    }

    /// Bresenham line, both endpoints inclusive; segments outside the image are clipped.
    void drawLine(int x0, int y0, int x1, int y1, Color c);

    /// Direct access to all pixels, row by row — for tight loops.
    Color* pixels() { return pixels_.data(); }
    /// @copydoc pixels()
    [[nodiscard]] const Color* pixels() const { return pixels_.data(); }
    /// Raw RGBA bytes (4 per pixel), the format sf::Texture::update() and sf::Image expect.
    [[nodiscard]] const std::uint8_t* bytes() const {
        // Color is exactly 4 x uint8_t (static_assert in Color.hpp), so this byte view is well-defined.
        return reinterpret_cast<const std::uint8_t*>(pixels_.data()); // NOLINT(*-reinterpret-cast)
    }

private:
    int width_ = 0;
    int height_ = 0;
    std::vector<Color> pixels_;
};
