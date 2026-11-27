#include "complex/MandelbrotRenderer.hpp"

#include <imgui.h>

// Start view: the whole set lies in Re in [-2, 0.5], Im in [-1.2, 1.2]; add a small margin.
MandelbrotRenderer::MandelbrotRenderer() : ComplexRenderer(-0.75, 0.0, 3.0, 2.6) {}

EscapeResult MandelbrotRenderer::escapeAt(double re, double im) const {
    if (insideMainCardioidOrBulb(re, im))
        return {maxIterations(), 0.0, false};
    return iterateQuadratic(0.0, 0.0, re, im, maxIterations(), kSmoothBailoutSq);
}

bool MandelbrotRenderer::onClick(MouseButton button, int px, int py) {
    if (button == MouseButton::Right && onPickC)
        onPickC(pixelRe(px), pixelIm(py));
    return false;
}

bool MandelbrotRenderer::drawExtraControls() {
    ImGui::TextWrapped("Right click a point to open the Julia set for that c.");
    return false;
}
