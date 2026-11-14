#include "lsystem/LSystemRenderer.hpp"

#include "core/LineReader.hpp"

#include <imgui.h>

#include <stdexcept>

void LSystemRenderer::render(Image& image) {
    // TODO: string rewriting, turtle graphics with stack, fit to window, image.drawLine().
    image.fill(Color(20, 20, 30));
    image.drawLine(0, 0, image.width() - 1, image.height() - 1, Color(220, 220, 220));
    image.drawLine(0, image.height() - 1, image.width() - 1, 0, Color(220, 220, 220));
}

bool LSystemRenderer::drawControls() {
    ImGui::TextWrapped("TODO: axiom, rules, angle, iteration count, built-in curves.");
    return false;
}

void LSystemRenderer::loadDefinition(const std::string& path) {
    // TODO: parser — lines "axiom ...", "rule X=...", "angle ...", "iterations ..." (in.line()).
    LineReader in = LineReader::fromFile(path);
    (void)in;
    throw ParseError(0, "Loading .lsys is not implemented yet");
}

void LSystemRenderer::saveDefinition(const std::string& path) const {
    (void)path;
    throw std::runtime_error("Saving .lsys is not implemented yet");
}
