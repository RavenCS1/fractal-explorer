#include "complex/JuliaRenderer.hpp"

#include <imgui.h>

#include <array>

namespace {

struct JuliaPreset {
    const char* name;
    double re;
    double im;
};

constexpr std::array<JuliaPreset, 5> kPresets = {
    {
        {"Default", -0.8, 0.156},
        {"Douady rabbit", -0.123, 0.745},
        {"Dendrite (c = i)", 0.0, 1.0},
        {"San Marco", -0.75, 0.0},
        {"Siegel disk", -0.391, -0.587},
    },
};

} // namespace

// Every Julia set lies in |z| <= 2, so a 4 x 3 window centered at 0 shows all of it.
JuliaRenderer::JuliaRenderer() : ComplexRenderer(0.0, 0.0, 4.0, 3.0) {}

EscapeResult JuliaRenderer::escapeAt(double re, double im) const {
    return iterateQuadratic(re, im, cRe_, cIm_, maxIterations(), kSmoothBailoutSq);
}

bool JuliaRenderer::drawExtraControls() {
    bool changed = false;
    changed |= ImGui::InputDouble("Re(c)", &cRe_, 0.001, 0.01, "%.6f");
    changed |= ImGui::InputDouble("Im(c)", &cIm_, 0.001, 0.01, "%.6f");
    if (ImGui::BeginCombo("Presets", "choose c...")) {
        for (const JuliaPreset& p : kPresets) {
            if (ImGui::Selectable(p.name)) {
                setC(p.re, p.im);
                changed = true;
            }
        }
        ImGui::EndCombo();
    }
    ImGui::Separator();
    return changed;
}
