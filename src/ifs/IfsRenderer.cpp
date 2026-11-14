#include "ifs/IfsRenderer.hpp"

#include "core/LineReader.hpp"

#include <imgui.h>

#include <stdexcept>

void IfsRenderer::render(Image& image) {
    // TODO: gra w chaos, kolor wg przeksztalcenia, automatyczne dopasowanie skali.
    for (int y = 0; y < image.height(); ++y)
        for (int x = 0; x < image.width(); ++x)
            image.set(x, y, Color(static_cast<std::uint8_t>(x), 160, static_cast<std::uint8_t>(y)));
}

bool IfsRenderer::drawControls() {
    ImGui::TextWrapped("TODO: wbudowane fraktale, tabela wspolczynnikow a b c d e f p, animacja przejscia.");
    return false;
}

void IfsRenderer::loadDefinition(const std::string& path) {
    // TODO: parser - kazdy wiersz to "a b c d e f p" (in.numbers(7)).
    LineReader in = LineReader::fromFile(path);
    (void)in;
    throw ParseError(0, "Wczytywanie .ifs jeszcze nie jest zaimplementowane");
}

void IfsRenderer::saveDefinition(const std::string& path) const {
    (void)path;
    throw std::runtime_error("Zapis .ifs jeszcze nie jest zaimplementowany");
}
