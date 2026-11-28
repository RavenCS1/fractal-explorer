#pragma once

/**
 * @file
 * L-system mode (turtle graphics).
 */

#include "core/FractalRenderer.hpp"

/**
 * Draws an LSystem with turtle graphics.
 *
 * Owner: Patryk.
 * @todo Stub: rendering, controls and load/save are not implemented yet.
 */
class LSystemRenderer : public FractalRenderer {
public:
    [[nodiscard]] const char* name() const override { return "L-system"; }
    void render(Image& image) override;
    bool drawControls() override;

    [[nodiscard]] const char* fileFilter() const override { return ".lsys"; }
    void loadDefinition(const std::string& path) override;
    void saveDefinition(const std::string& path) const override;
};
