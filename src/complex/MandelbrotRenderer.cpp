#include "complex/MandelbrotRenderer.hpp"

#include <imgui.h>

void MandelbrotRenderer::render(Image& image) {
    // TODO: odwzorowanie piksel -> plaszczyzna zespolona, iteracja z^2 + c, plynne kolorowanie.
    for (int y = 0; y < image.height(); ++y)
        for (int x = 0; x < image.width(); ++x)
            image.set(x, y, Color(static_cast<std::uint8_t>(x), static_cast<std::uint8_t>(y), 160));
}

bool MandelbrotRenderer::drawControls() {
    ImGui::TextWrapped("TODO: iteracje, paleta. Nawigacja: onZoom / onDrag / onClick / resetView.");
    return false;
}
