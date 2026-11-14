#pragma once

#include "core/FractalRenderer.hpp"

// ZASLEPKA - wlasciciel: Marcel.
// Animacja przejscia: mozna ja zrobic tutaj (sekcja "Animacja" w drawControls)
// albo jako osobny renderer, np. IfsMorphRenderer. Kolejne klatki liczy tick() -
// jedna klatka animacji na jedno wywolanie, wtedy okno pokazuje postep bez watkow.
class IfsRenderer : public FractalRenderer {
public:
    const char* name() const override { return "IFS"; }
    void render(Image& image) override;
    bool drawControls() override;

    const char* fileFilter() const override { return ".ifs"; }
    void loadDefinition(const std::string& path) override;
    void saveDefinition(const std::string& path) const override;
};
