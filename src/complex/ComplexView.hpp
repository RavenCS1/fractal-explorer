#pragma once

/**
 * @file
 * Mapping between image pixels and points of the complex plane.
 */

/**
 * The visible part of the complex plane: where the window is centered and how zoomed in it is.
 *
 * A pixel (px, py) of a W x H image corresponds to the complex number x + iy, where
 *
 *     x = centerX + (px - W/2) * scale
 *     y = centerY - (py - H/2) * scale      (minus: pixel y grows downwards, imaginary axis grows upwards)
 *
 * so the image center always shows (centerX, centerY), and @ref scale is the size of one pixel
 * in complex units. Zooming in = making @ref scale smaller.
 */
struct ComplexView {
    /// Smallest allowed pixel size. Below ~1e-15 neighbouring pixels map to the same double, and the image
    /// turns into blocks; we stop zooming a bit earlier than that.
    static constexpr double kMinScale = 1e-14;
    /// Largest allowed pixel size (the whole interesting part of both fractals fits in |z| < 2).
    static constexpr double kMaxScale = 0.1;

    double centerX = 0.0; ///< Real part of the point shown in the middle of the image.
    double centerY = 0.0; ///< Imaginary part of the point shown in the middle of the image.
    double scale = 0.005; ///< Width (and height) of one pixel in complex-plane units.

    /// Real part of the complex number under pixel column @p px of an image @p width pixels wide.
    [[nodiscard]] double toRe(double px, int width) const { return centerX + ((px - (width / 2.0)) * scale); }
    /// Imaginary part of the complex number under pixel row @p py of an image @p height pixels tall.
    [[nodiscard]] double toIm(double py, int height) const { return centerY - ((py - (height / 2.0)) * scale); }

    /**
     * Zooms by @p factor so that the point under pixel (@p px, @p py) stays under the same pixel.
     *
     * @param factor  > 1 zooms in, < 1 zooms out (e.g. 1.25 = everything looks 25% larger).
     * @param px,py   Pixel under the mouse cursor.
     * @param width,height  Image size in pixels.
     * @return false when the zoom limit was already reached and nothing changed.
     */
    bool zoomAt(double factor, int px, int py, int width, int height);

    /**
     * Moves the view as if the image was dragged by (@p dx, @p dy) pixels:
     * the point under the cursor follows the cursor.
     */
    void pan(int dx, int dy);

    /**
     * Returns a view centered at (@p centerX, @p centerY) that shows at least @p spanX x @p spanY
     * complex units in an image of @p width x @p height pixels (keeping pixels square).
     */
    [[nodiscard]] static ComplexView fit(double centerX, double centerY, double spanX, double spanY, int width,
                                         int height);
};
