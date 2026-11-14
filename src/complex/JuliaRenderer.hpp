#pragma once

#include "core/FractalRenderer.hpp"

// STUB — owner: Jakub.
// Picking c by clicking the Mandelbrot set: e.g. MandelbrotRenderer exposes
// std::function<void(double re, double im)> onPickC, and App wires it to setC() + mode switch.
class JuliaRenderer : public FractalRenderer {
public:
    const char* name() const override { return "Julia"; }
    void render(Image& image) override;
    bool drawControls() override;

    void setC(double re, double im) {
        cRe_ = re;
        cIm_ = im;
    }

private:
    double cRe_ = -0.8;
    double cIm_ = 0.156;
};
