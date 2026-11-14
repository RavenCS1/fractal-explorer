#include "ifs/IfsRenderer.hpp"

#include "core/LineReader.hpp"

#include <imgui.h>

#include <stdexcept>

void IfsRenderer::render(Image& image) {
    // TODO: chaos game, color by transform, automatic scale fitting.
    for (int y = 0; y < image.height(); ++y)
        for (int x = 0; x < image.width(); ++x)
            image.set(x, y, Color(static_cast<std::uint8_t>(x), 160, static_cast<std::uint8_t>(y)));
}

bool IfsRenderer::drawControls() {
    ImGui::TextWrapped("TODO: built-in fractals, coefficient table a b c d e f p, transition animation.");
    return false;
}

void IfsRenderer::loadDefinition(const std::string& path) {
    // TODO: parser — each line is "a b c d e f p" (in.numbers(7)).
    LineReader in = LineReader::fromFile(path);
    (void)in;
    throw ParseError(0, "Loading .ifs is not implemented yet");
}

void IfsRenderer::saveDefinition(const std::string& path) const {
    (void)path;
    throw std::runtime_error("Saving .ifs is not implemented yet");
}
