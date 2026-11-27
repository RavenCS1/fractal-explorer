#pragma once

/**
 * @file
 * Julia sets.
 */

#include "complex/ComplexRenderer.hpp"

/**
 * The Julia set for a fixed c: the starting points z_0 for which z_{n+1} = z_n^2 + c stays bounded.
 *
 * Same formula as the Mandelbrot set, roles swapped: here the pixel is z_0 and c is one constant for the
 * whole image. If c lies inside the Mandelbrot set the Julia set is one connected shape; if c is outside,
 * it falls apart into "dust". The most interesting ones come from c near the Mandelbrot set's edge.
 *
 * Owner: Jakub.
 */
class JuliaRenderer : public ComplexRenderer {
public:
    JuliaRenderer();

    [[nodiscard]] const char* name() const override { return "Julia"; }

    /// Sets the constant c = @p re + i * @p im (e.g. picked by right-clicking the Mandelbrot set).
    void setC(double re, double im) {
        cRe_ = re;
        cIm_ = im;
    }
    [[nodiscard]] double cRe() const { return cRe_; }
    [[nodiscard]] double cIm() const { return cIm_; }

protected:
    [[nodiscard]] EscapeResult escapeAt(double re, double im) const override;
    bool drawExtraControls() override;

private:
    double cRe_ = -0.8;
    double cIm_ = 0.156;
};
