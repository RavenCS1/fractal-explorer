#pragma once

#include "ifs/AffineTransform.hpp"
#include "core/FractalRenderer.hpp"
#include <vector>

class IfsRenderer : public FractalRenderer {
public:
    [[nodiscard]] const char* name() const override { return "IFS"; }
    void render(Image& image) override;
    bool drawControls() override;

    [[nodiscard]] const char* fileFilter() const override { return ".ifs"; }
    void loadDefinition(const std::string& path) override;
    void saveDefinition(const std::string& path) const override;

private:
	std::vector<AffineTransform> transforms_;
};
