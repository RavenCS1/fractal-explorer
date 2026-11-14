#include "complex/JuliaRenderer.hpp"

#include <imgui.h>

void JuliaRenderer::render(Image& image) {
    // TODO: iterate z^2 + c, where z0 = pixel coordinate, c is constant.
    for (int y = 0; y < image.height(); ++y)
        for (int x = 0; x < image.width(); ++x)
            image.set(x, y, Color(160, static_cast<std::uint8_t>(x), static_cast<std::uint8_t>(y)));
}

bool JuliaRenderer::drawControls() {
    bool changed = false;
    changed |= ImGui::InputDouble("Re(c)", &cRe_, 0.001, 0.01, "%.5f");
    changed |= ImGui::InputDouble("Im(c)", &cIm_, 0.001, 0.01, "%.5f");
    ImGui::TextWrapped("TODO: iterations, palette.");
    return changed;
}
