#pragma once

/**
 * @file
 * The Mandelbrot set.
 */

#include "complex/ComplexRenderer.hpp"

#include <functional>

/**
 * The Mandelbrot set: the points c for which z_{n+1} = z_n^2 + c, starting at z_0 = 0, stays bounded.
 *
 * Right-clicking a point calls @ref onPickC with its coordinates; the application uses that to open the
 * Julia set for that c (every point of the Mandelbrot set "contains" one Julia set).
 *
 * Owner: Jakub.
 */
class MandelbrotRenderer : public ComplexRenderer {
public:
    MandelbrotRenderer();

    [[nodiscard]] const char* name() const override { return "Mandelbrot"; }
    bool onClick(MouseButton button, int px, int py) override;

    /// Called on right click with the complex coordinates of the clicked point. May be empty.
    std::function<void(double re, double im)> onPickC;

protected:
    [[nodiscard]] EscapeResult escapeAt(double re, double im) const override;
    bool drawExtraControls() override;
};
