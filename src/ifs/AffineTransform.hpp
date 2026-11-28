#pragma once

/**
 * @file
 * One transformation of an IFS.
 */

#include "ifs/Point.hpp"

/**
 * Affine map x' = a*x + b*y + e, y' = c*x + d*y + f, chosen in the chaos game with probability p.
 * Fields are in the same order as one line of an .ifs file: a b c d e f p.
 */
struct AffineTransform {
    double a; ///< x coefficient of x'.
    double b; ///< y coefficient of x'.
    double c; ///< x coefficient of y'.
    double d; ///< y coefficient of y'.
    double e; ///< Translation in x.
    double f; ///< Translation in y.
    double p; ///< Relative probability of choosing this transform (need not sum to 1).

    /// Applies the transform to @p point.

    [[nodiscard]] Point apply(const Point& point) const {
        return {(a * point.x) + (b * point.y) + e, (c * point.x) + (d * point.y) + f};
    }
};
