#pragma once

/**
 * @file
 * IFS fractals drawn with the chaos game.
 */

#include "core/FractalRenderer.hpp"
#include "ifs/AffineTransform.hpp"

#include <random>
#include <vector>

/**
 * Iterated function system (IFS) fractals such as the Sierpinski triangle or the Barnsley fern.
 *
 * Drawn with the chaos game: start anywhere, repeatedly apply a randomly chosen affine transform
 * (picked with its probability p) and plot each point, colored by the transform that produced it.
 * The image is scaled automatically to fit the attractor.
 *
 * Owner: Marcel.
 */
class IfsRenderer : public FractalRenderer {
public:
    [[nodiscard]] const char* name() const override { return "IFS"; }
    void render(Image& image) override;
    bool drawControls() override;

    [[nodiscard]] const char* fileFilter() const override { return ".ifs"; }
    void loadDefinition(const std::string& path) override;
    void saveDefinition(const std::string& path) const override;

private:
    std::vector<AffineTransform> transforms_; ///< The loaded definition, one entry per line of the .ifs file.
    static constexpr int kRngSeed = 4;
    std::mt19937 rng_{kRngSeed}; // replace seed later
};
