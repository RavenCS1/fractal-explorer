#pragma once

#include "core/FractalRenderer.hpp"

// STUB — owner: Marcel.
// Transition animation: can be done here (an "Animation" section in drawControls)
// or as a separate renderer, e.g. IfsMorphRenderer. Frames are computed in tick() —
// one animation frame per call, so the window shows progress without threads.
class IfsRenderer : public FractalRenderer {
public:
    const char* name() const override { return "IFS"; }
    void render(Image& image) override;
    bool drawControls() override;

    const char* fileFilter() const override { return ".ifs"; }
    void loadDefinition(const std::string& path) override;
    void saveDefinition(const std::string& path) const override;
};
