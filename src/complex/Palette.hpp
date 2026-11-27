#pragma once

/**
 * @file
 * Color gradients for the complex fractals.
 */

#include "core/Color.hpp"

#include <vector>

/**
 * A cyclic color gradient defined by control points ("stops").
 *
 * sample(t) blends linearly between neighbouring stops. The gradient wraps around: after the last stop it
 * blends back into the first one, so sample(0.999) is close to sample(0.0). That matters because the
 * smooth iteration count keeps growing, and we map it onto the palette repeatedly (modulo 1); a seam
 * at t = 1 would show up as a hard edge in the image.
 */
class Palette {
public:
    /// One control point of the gradient.
    struct Stop {
        double position = 0.0; ///< Where this color sits, in [0, 1).
        Color color;           ///< The color at that position.
    };

    /// @param name  Label shown in the panel.
    /// @param stops Control points sorted by position; must not be empty.
    Palette(const char* name, std::vector<Stop> stops);

    /// Color at position @p t; any real number is accepted, only its fractional part matters.
    [[nodiscard]] Color sample(double t) const;

    [[nodiscard]] const char* name() const { return name_; }

    /// The built-in palettes offered in the panel (at least three, per the requirements).
    [[nodiscard]] static const std::vector<Palette>& builtins();

private:
    const char* name_;
    std::vector<Stop> stops_;
};
