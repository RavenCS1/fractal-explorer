#pragma once

#include "core/FractalRenderer.hpp"

// STUB — owner: Jakub.
class MandelbrotRenderer : public FractalRenderer {
public:
    const char* name() const override { return "Mandelbrot"; }
    void render(Image& image) override;
    bool drawControls() override;
};
