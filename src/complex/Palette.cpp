#include "complex/Palette.hpp"

#include <cmath>
#include <cstdint>
#include <utility>

namespace {

std::uint8_t lerpChannel(std::uint8_t a, std::uint8_t b, double t) {
    return static_cast<std::uint8_t>(std::lround(a + ((b - a) * t)));
}

Color lerp(Color a, Color b, double t) {
    return {lerpChannel(a.r, b.r, t), lerpChannel(a.g, b.g, t), lerpChannel(a.b, b.b, t)};
}

} // namespace

Palette::Palette(const char* name, std::vector<Stop> stops) : name_(name), stops_(std::move(stops)) {}

Color Palette::sample(double t) const {
    if (stops_.empty())
        return {};
    t -= std::floor(t); // keep only the fractional part: [0, 1)

    // Find the first stop after t; we blend between the one before it and that one.
    std::size_t next = 0;
    while (next < stops_.size() && stops_[next].position <= t)
        ++next;

    // Wrap around: before the first stop we come from the last one, after the last we go to the first.
    const Stop& from = stops_[(next + stops_.size() - 1) % stops_.size()];
    const Stop& to = stops_[next % stops_.size()];
    double span = to.position - from.position;
    double offset = t - from.position;
    if (span <= 0.0)
        span += 1.0;
    if (offset < 0.0)
        offset += 1.0;
    return lerp(from.color, to.color, offset / span);
}

const std::vector<Palette>& Palette::builtins() {
    static const std::vector<Palette> kPalettes = {
        {
            "Classic",
            {
                {0.0, {0, 7, 100}},
                {0.16, {32, 107, 203}},
                {0.42, {237, 255, 255}},
                {0.6425, {255, 170, 0}},
                {0.8575, {0, 2, 0}},
            },
        },
        {
            "Fire",
            {
                {0.0, {10, 0, 0}},
                {0.3, {180, 20, 0}},
                {0.55, {255, 140, 0}},
                {0.75, {255, 240, 120}},
            },
        },
        {
            "Ocean",
            {
                {0.0, {0, 10, 40}},
                {0.35, {0, 90, 140}},
                {0.6, {60, 200, 190}},
                {0.8, {230, 255, 250}},
            },
        },
        {
            "Grayscale",
            {
                {0.0, {0, 0, 0}},
                {0.5, {255, 255, 255}},
            },
        },
    };
    return kPalettes;
}
