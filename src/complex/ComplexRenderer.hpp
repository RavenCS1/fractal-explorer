#pragma once

/**
 * @file
 * Shared base for the Mandelbrot and Julia renderers.
 */

#include "complex/ComplexView.hpp"
#include "complex/Escape.hpp"
#include "core/FractalRenderer.hpp"

#include <string>

/**
 * Everything the Mandelbrot and Julia sets have in common: the view (zoom/pan), iteration limit,
 * palette, coloring, the per-pixel render loop and the shared panel widgets.
 *
 * The two fractals differ in one thing only: which number is z_0 and which is c (see escapeAt()).
 */
class ComplexRenderer : public FractalRenderer {
public:
    void render(Image& image) override;
    bool drawControls() override;
    bool onZoom(double factor, int px, int py) override;
    bool onDrag(int dx, int dy) override;
    void resetView() override;
    [[nodiscard]] std::string statusText(int px, int py) const override;

    [[nodiscard]] const ComplexView& view() const { return view_; }
    [[nodiscard]] int maxIterations() const { return maxIterations_; }
    void setMaxIterations(int n);

protected:
    /**
     * @param homeX,homeY  Center of the start view.
     * @param homeSpanX,homeSpanY  Area (in complex units) the start view must show.
     */
    ComplexRenderer(double homeX, double homeY, double homeSpanX, double homeSpanY);

    /// Iterates the point (@p re, @p im) of the plane. This is the only fractal-specific part.
    [[nodiscard]] virtual EscapeResult escapeAt(double re, double im) const = 0;

    /// Extra widgets drawn above the shared ones; returns true when the image must be recalculated.
    virtual bool drawExtraControls() { return false; }

    /// Complex coordinates of a pixel in the last rendered image.
    [[nodiscard]] double pixelRe(int px) const { return view_.toRe(px, width_); }
    [[nodiscard]] double pixelIm(int py) const { return view_.toIm(py, height_); }

private:
    [[nodiscard]] Color colorFor(const EscapeResult& r) const;

    double homeX_, homeY_, homeSpanX_, homeSpanY_;
    ComplexView view_;
    bool needsFit_ = true; ///< Fit the start view on the next render (size is unknown until then).
    int width_ = 1;        ///< Size of the last rendered image, needed to map mouse pixels.
    int height_ = 1;

    int maxIterations_ = 256;
    int palette_ = 0;           ///< Index into Palette::builtins().
    float colorDensity_ = 1.0F; ///< Palette cycles per 32 iterations.
};
