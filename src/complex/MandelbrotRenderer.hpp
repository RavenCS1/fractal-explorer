#pragma once

#include "core/FractalRenderer.hpp"

// ZASLEPKA - wlasciciel: Jakub.
class MandelbrotRenderer : public FractalRenderer {
public:
    const char* name() const override { return "Mandelbrot"; }
    void render(Image& image) override;
    bool drawControls() override;
};
