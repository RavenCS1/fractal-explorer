#include "core/Image.hpp"

#include <algorithm>
#include <cstdlib>

Image::Image(int width, int height, Color fill) { resize(width, height, fill); }

void Image::resize(int width, int height, Color fill) {
    width_ = std::max(0, width);
    height_ = std::max(0, height);
    pixels_.assign(static_cast<std::size_t>(width_) * height_, fill);
}

void Image::fill(Color c) { std::fill(pixels_.begin(), pixels_.end(), c); }

void Image::drawLine(int x0, int y0, int x1, int y1, Color c) {
    const int dx = std::abs(x1 - x0);
    const int dy = -std::abs(y1 - y0);
    const int sx = x0 < x1 ? 1 : -1;
    const int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;

    while (true) {
        set(x0, y0, c);
        if (x0 == x1 && y0 == y1) break;
        const int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}
