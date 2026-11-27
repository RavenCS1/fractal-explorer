#include "complex/ComplexView.hpp"

#include <algorithm>

bool ComplexView::zoomAt(double factor, int px, int py, int width, int height) {
    if (factor <= 0.0 || width <= 0 || height <= 0)
        return false;

    const double newScale = std::clamp(scale / factor, kMinScale, kMaxScale);
    if (newScale == scale)
        return false;

    // q = point under the cursor. It must stay put, so every point m moves towards q:
    //   m' = q + (m - q) * (newScale / scale)
    // Applied to the center this gives the new center.
    const double qx = toRe(px, width);
    const double qy = toIm(py, height);
    const double ratio = newScale / scale;
    centerX = qx + ((centerX - qx) * ratio);
    centerY = qy + ((centerY - qy) * ratio);
    scale = newScale;
    return true;
}

void ComplexView::pan(int dx, int dy) {
    // Dragging right shows what was on the left: the center moves left. Pixel y is flipped (see toIm).
    centerX -= dx * scale;
    centerY += dy * scale;
}

ComplexView ComplexView::fit(double centerX, double centerY, double spanX, double spanY, int width, int height) {
    ComplexView view;
    view.centerX = centerX;
    view.centerY = centerY;
    const double w = std::max(1, width);
    const double h = std::max(1, height);
    // The larger of the two ratios guarantees both spans fit.
    view.scale = std::clamp(std::max(spanX / w, spanY / h), kMinScale, kMaxScale);
    return view;
}
