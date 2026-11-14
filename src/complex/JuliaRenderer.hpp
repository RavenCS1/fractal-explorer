#pragma once

#include "core/FractalRenderer.hpp"

// ZASLEPKA - wlasciciel: Jakub.
// Wybor c kliknieciem na zbiorze Mandelbrota: np. MandelbrotRenderer udostepnia
// std::function<void(double re, double im)> onPickC, a App podpina pod nia setC() + przelaczenie trybu.
class JuliaRenderer : public FractalRenderer {
public:
    const char* name() const override { return "Julia"; }
    void render(Image& image) override;
    bool drawControls() override;

    void setC(double re, double im) { cRe_ = re; cIm_ = im; }

private:
    double cRe_ = -0.8;
    double cIm_ = 0.156;
};
