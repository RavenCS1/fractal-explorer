#pragma once

/**
 * @file
 * The core of the Mandelbrot and Julia sets: iterating z -> z^2 + c and turning the result into a
 * smooth (band-free) number used for coloring.
 */

#include <cmath>

/// What happened when iterating one point.
struct EscapeResult {
    int iterations = 0;       ///< Steps taken before |z| exceeded the bailout radius (or the limit).
    double magnitudeSq = 0.0; ///< |z|^2 at the moment of escape; needed by smoothIterationCount().
    bool escaped = false;     ///< false = the point stayed bounded, i.e. it is (probably) in the set.
};

/**
 * Bailout radius^2 used for drawing. Mathematically radius 2 (|z|^2 > 4) is enough to know a point
 * escapes, but a bigger radius makes smoothIterationCount() much smoother, at the cost of 2-3 extra steps.
 */
constexpr double kSmoothBailoutSq = 256.0 * 256.0;

/**
 * Iterates z_{n+1} = z_n^2 + c starting at z_0, until |z|^2 > @p bailoutSq or @p maxIterations steps.
 *
 * Complex numbers are kept as two doubles: z = zr + i*zi. Squaring gives
 *     (zr + i*zi)^2 = (zr^2 - zi^2) + i*(2*zr*zi)
 * which is where the two update lines below come from.
 *
 * - Mandelbrot set: z_0 = 0, c = the pixel's coordinate.
 * - Julia set:      z_0 = the pixel's coordinate, c = one constant for the whole image.
 */
inline EscapeResult iterateQuadratic(double zr, double zi, double cr, double ci, int maxIterations, double bailoutSq) {
    for (int n = 0; n < maxIterations; ++n) {
        const double zr2 = zr * zr;
        const double zi2 = zi * zi;
        if (zr2 + zi2 > bailoutSq)
            return {n, zr2 + zi2, true};
        zi = (2.0 * zr * zi) + ci;
        zr = zr2 - zi2 + cr;
    }
    return {maxIterations, (zr * zr) + (zi * zi), false};
}

/**
 * Turns the whole iteration count n into a continuous value
 *
 *     nu = n + 1 - log2(ln|z_n|)
 *
 * For points that escape, |z| roughly squares every step, so ln|z| doubles and log2(ln|z|) grows by 1 per step.
 * Subtracting it cancels the jump of n by 1 between neighbouring "bands", which is what removes the visible
 * stripes. Only valid when @p r.escaped is true.
 */
inline double smoothIterationCount(const EscapeResult& r) {
    const double logModulus = 0.5 * std::log(r.magnitudeSq); // ln|z| = ln(|z|^2) / 2
    return r.iterations + 1.0 - std::log2(logModulus);
}

/**
 * Cheap test for the two biggest blobs of the Mandelbrot set (the main cardioid and the period-2 bulb).
 * Points inside them never escape, so we can skip up to maxIterations steps; this makes the default view
 * several times faster.
 */
inline bool insideMainCardioidOrBulb(double x, double y) {
    const double y2 = y * y;
    const double xq = x - 0.25;
    const double q = (xq * xq) + y2;
    if (q * (q + xq) <= 0.25 * y2)
        return true;
    const double xb = x + 1.0;
    return (xb * xb) + y2 <= 1.0 / 16.0;
}
