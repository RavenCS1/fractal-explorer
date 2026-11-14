#pragma once

#include "core/FractalRenderer.hpp"

// ZASLEPKA - wlasciciel: Patryk.
class LSystemRenderer : public FractalRenderer {
public:
    const char* name() const override { return "L-system"; }
    void render(Image& image) override;
    bool drawControls() override;

    const char* fileFilter() const override { return ".lsys"; }
    void loadDefinition(const std::string& path) override;
    void saveDefinition(const std::string& path) const override;
};
