#include "complex/ComplexRenderer.hpp"

#include "complex/Palette.hpp"

#include <imgui.h>

#include <algorithm>
#include <array>
#include <cstdio>

namespace {
constexpr int kMinIterations = 16;
constexpr int kMaxIterationsLimit = 10000;
constexpr double kIterationsPerCycle = 32.0; // at density 1, the palette repeats every 32 iterations
constexpr Color kInsideColor(0, 0, 0);
} // namespace

ComplexRenderer::ComplexRenderer(double homeX, double homeY, double homeSpanX, double homeSpanY)
    : homeX_(homeX), homeY_(homeY), homeSpanX_(homeSpanX), homeSpanY_(homeSpanY) {}

void ComplexRenderer::setMaxIterations(int n) {
    maxIterations_ = std::clamp(n, kMinIterations, kMaxIterationsLimit);
}

void ComplexRenderer::render(Image& image) {
    width_ = std::max(1, image.width());
    height_ = std::max(1, image.height());
    if (needsFit_) {
        view_ = ComplexView::fit(homeX_, homeY_, homeSpanX_, homeSpanY_, width_, height_);
        needsFit_ = false;
    }

    Color* pixels = image.pixels();
    for (int py = 0; py < image.height(); ++py) {
        const double im = view_.toIm(py, height_);
        Color* row = pixels + (static_cast<std::size_t>(py) * image.width());
        for (int px = 0; px < image.width(); ++px)
            row[px] = colorFor(escapeAt(view_.toRe(px, width_), im));
    }
}

Color ComplexRenderer::colorFor(const EscapeResult& r) const {
    if (!r.escaped)
        return kInsideColor;
    const double nu = smoothIterationCount(r);
    const auto& palettes = Palette::builtins();
    return palettes[static_cast<std::size_t>(palette_)].sample(nu * colorDensity_ / kIterationsPerCycle);
}

bool ComplexRenderer::onZoom(double factor, int px, int py) {
    return view_.zoomAt(factor, px, py, width_, height_);
}

bool ComplexRenderer::onDrag(int dx, int dy) {
    view_.pan(dx, dy);
    return true;
}

void ComplexRenderer::resetView() {
    needsFit_ = true;
}

std::string ComplexRenderer::statusText(int px, int py) const {
    std::array<char, 128> buf{};
    (void)std::snprintf(buf.data(), buf.size(), "x = %.10g, y = %.10g    scale: %.2e", pixelRe(px), pixelIm(py),
                        view_.scale);
    return buf.data();
}

bool ComplexRenderer::drawControls() {
    bool changed = drawExtraControls();

    int iterations = maxIterations_;
    if (ImGui::SliderInt("Iterations", &iterations, kMinIterations, kMaxIterationsLimit, "%d",
                         ImGuiSliderFlags_Logarithmic)) {
        setMaxIterations(iterations);
        changed = true;
    }

    const auto& palettes = Palette::builtins();
    if (ImGui::BeginCombo("Palette", palettes[static_cast<std::size_t>(palette_)].name())) {
        for (int i = 0; i < static_cast<int>(palettes.size()); ++i) {
            if (ImGui::Selectable(palettes[static_cast<std::size_t>(i)].name(), i == palette_)) {
                changed |= (i != palette_);
                palette_ = i;
            }
        }
        ImGui::EndCombo();
    }
    changed |= ImGui::SliderFloat("Color density", &colorDensity_, 0.1F, 8.0F, "%.2f", ImGuiSliderFlags_Logarithmic);

    if (ImGui::Button("Reset view")) {
        resetView();
        changed = true;
    }
    if (view_.scale <= ComplexView::kMinScale)
        ImGui::TextColored(ImVec4(1.F, 0.7F, 0.3F, 1.F), "Zoom limit of double precision reached.");
    ImGui::TextDisabled("Wheel or +/-: zoom  |  Drag or arrows: pan  |  R: reset");
    return changed;
}
