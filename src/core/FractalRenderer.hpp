#pragma once

#include "core/Image.hpp"

#include <string>

enum class MouseButton { Left, Right, Middle };

// Common interface for all fractal types.
// The application holds a list of renderers and always talks to the active one through these methods.
//
// RULES:
//  1. render() only computes and draws to Image. No ImGui or SFML inside.
//  2. drawControls() only draws widgets and changes parameters. No computation.
//  3. Methods returning bool return true when the image needs to be recalculated;
//     the application then calls render().
//  4. Pixel coordinates (px, py) are relative to the top-left corner
//     of the fractal area, not the entire window.
class FractalRenderer {
public:
    virtual ~FractalRenderer() = default;

    // Mode name in the selection list, e.g. "Mandelbrot".
    virtual const char* name() const = 0;

    // Renders the full image. Image size = fractal area size;
    // on window resize the application passes an image of the new size.
    virtual void render(Image& image) = 0;

    // ImGui widgets in the control panel (parameters, palettes, buttons).
    // Called once per frame, inside the panel window.
    virtual bool drawControls() = 0;

    // Called once per frame of the main loop. For work spread across multiple frames,
    // e.g. animation generation: one animation frame per call.
    virtual bool tick() { return false; }

    // --- Navigation (no-op by default) -------------------------------------
    // factor > 1 zooms in, factor < 1 zooms out; (px, py) is the point under the cursor.
    virtual bool onZoom(double factor, int px, int py) {
        (void)factor;
        (void)px;
        (void)py;
        return false;
    }
    // Mouse drag by (dx, dy) pixels with the left button held.
    virtual bool onDrag(int dx, int dy) {
        (void)dx;
        (void)dy;
        return false;
    }
    // Click without dragging.
    virtual bool onClick(MouseButton button, int px, int py) {
        (void)button;
        (void)px;
        (void)py;
        return false;
    }
    // Return to the initial view (View menu).
    virtual void resetView() {}

    // Status bar text for the pixel under the cursor, e.g. "x = -0.7436, y = 0.1318".
    virtual std::string statusText(int px, int py) const {
        (void)px;
        (void)py;
        return {};
    }

    // --- Definition files ----------------------------------------------------
    // Filter for the file dialog, e.g. ".ifs". nullptr = this mode has no definition files.
    virtual const char* fileFilter() const { return nullptr; }
    // Throws ParseError (with line number) or std::runtime_error.
    virtual void loadDefinition(const std::string& path) { (void)path; }
    // Throws std::runtime_error on write failure.
    virtual void saveDefinition(const std::string& path) const { (void)path; }
};
