#pragma once

/**
 * @file
 * 2D point used by the IFS module.
 */

/// A point of the plane in IFS (not pixel) coordinates.
struct Point {
    double x{0.0}; ///< Horizontal coordinate.
    double y{0.0}; ///< Vertical coordinate (grows upwards).
};
