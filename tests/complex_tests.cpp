#include "TestUtil.hpp"
#include "complex/ComplexView.hpp"
#include "complex/Escape.hpp"
#include "complex/JuliaRenderer.hpp"
#include "complex/MandelbrotRenderer.hpp"
#include "complex/Palette.hpp"
#include "core/Image.hpp"

#include <cmath>
#include <cstdlib>

namespace {

// --- iteration (known results from the project plan) ---

void testKnownMandelbrotPoints() {
    // c = 0: z stays 0 forever -> in the set.
    CHECK(!iterateQuadratic(0, 0, 0, 0, 1000, 4.0).escaped);
    // c = -1: z cycles 0, -1, 0, -1 ... -> in the set.
    CHECK(!iterateQuadratic(0, 0, -1, 0, 1000, 4.0).escaped);
    // c = 1: z = 0, 1, 2, 5 -> |5|^2 = 25 > 4, detected after 3 steps.
    const EscapeResult r = iterateQuadratic(0, 0, 1, 0, 1000, 4.0);
    CHECK(r.escaped);
    CHECK(r.iterations == 3);
    CHECK_NEAR(r.magnitudeSq, 25.0, 1e-12);
    // c = i: z = 0, i, -1+i, -i, -1+i, ... -> bounded.
    CHECK(!iterateQuadratic(0, 0, 0, 1, 1000, 4.0).escaped);
}

void testCardioidShortcutAgreesWithIteration() {
    for (int i = 0; i <= 50; ++i) {
        for (int k = 0; k <= 48; ++k) {
            const double x = -2.0 + (i * 0.05);
            const double y = -1.2 + (k * 0.05);
            if (insideMainCardioidOrBulb(x, y)) {
                CHECK(!iterateQuadratic(0, 0, x, y, 2000, 4.0).escaped);
            }
        }
    }
    CHECK(insideMainCardioidOrBulb(0, 0));
    CHECK(insideMainCardioidOrBulb(-1, 0));
    CHECK(!insideMainCardioidOrBulb(1, 0));
}

void testSmoothCountIsContinuous() {
    // Walk along the real axis outside the set; integer counts jump, smooth values must not.
    double previous = -1.0;
    for (int i = 0; i < 2400; ++i) {
        const double x = 0.26 + (i * 1e-4);
        const EscapeResult r = iterateQuadratic(0, 0, x, 0, 10000, kSmoothBailoutSq);
        CHECK(r.escaped);
        const double nu = smoothIterationCount(r);
        if (previous >= 0.0)
            CHECK(std::abs(nu - previous) < 0.5);
        previous = nu;
    }
}

// --- view: pixel <-> complex plane, zoom, pan ---

void testViewMapping() {
    ComplexView v;
    v.centerX = -0.5;
    v.centerY = 0.25;
    v.scale = 0.01;
    CHECK_NEAR(v.toRe(50, 100), -0.5, 1e-12); // image center -> view center
    CHECK_NEAR(v.toIm(40, 80), 0.25, 1e-12);
    CHECK_NEAR(v.toRe(60, 100), -0.4, 1e-12); // 10 px right = +0.1
    CHECK_NEAR(v.toIm(30, 80), 0.35, 1e-12);  // 10 px up = +0.1 (y axis flipped)
}

void testZoomKeepsPointUnderCursor() {
    ComplexView v;
    v.scale = 0.01;
    const int w = 200;
    const int h = 100;
    const double beforeRe = v.toRe(30, w);
    const double beforeIm = v.toIm(70, h);
    CHECK(v.zoomAt(2.0, 30, 70, w, h));
    CHECK_NEAR(v.scale, 0.005, 1e-15);
    CHECK_NEAR(v.toRe(30, w), beforeRe, 1e-12);
    CHECK_NEAR(v.toIm(70, h), beforeIm, 1e-12);

    // Zoom limit: a huge factor clamps to kMinScale, and then nothing changes any more.
    v.zoomAt(1e30, 0, 0, w, h);
    CHECK_NEAR(v.scale, ComplexView::kMinScale, 1e-20);
    CHECK(!v.zoomAt(2.0, 0, 0, w, h));
}

void testPanFollowsCursor() {
    ComplexView v;
    v.scale = 0.01;
    const double re = v.toRe(10, 100);
    const double im = v.toIm(10, 100);
    v.pan(15, -5); // drag the image 15 px right and 5 px up
    CHECK_NEAR(v.toRe(25, 100), re, 1e-12);
    CHECK_NEAR(v.toIm(5, 100), im, 1e-12);
}

void testFitShowsWholeArea() {
    const ComplexView v = ComplexView::fit(-0.75, 0, 3.0, 2.6, 800, 400); // wide image: height decides
    CHECK_NEAR(v.scale, 2.6 / 400, 1e-12);
    CHECK(v.toRe(0, 800) <= -0.75 - 1.5);
    CHECK(v.toRe(800, 800) >= -0.75 + 1.5);
}

// --- palette ---

void testPaletteWrapsAround() {
    const Palette p("test", {{0.0, Color(0, 0, 0)}, {0.5, Color(200, 100, 50)}});
    CHECK(p.sample(0.0) == Color(0, 0, 0));
    CHECK(p.sample(0.5) == Color(200, 100, 50));
    CHECK(p.sample(0.25) == Color(100, 50, 25));
    CHECK(p.sample(0.75) == Color(100, 50, 25)); // blends back towards the first stop
    CHECK(p.sample(1.25) == p.sample(0.25));     // only the fractional part matters
    CHECK(p.sample(-0.75) == p.sample(0.25));
    CHECK(Palette::builtins().size() >= 3);
}

// --- renderers (headless: render() must not need a window) ---

void testMandelbrotRender() {
    MandelbrotRenderer m;
    Image img(120, 80);
    m.render(img);
    // Start view is centered at -0.75: the center pixel lies in the set (black),
    // the top-left corner (-2.25 + 1.3i) is far outside (colored).
    CHECK(img.get(60, 40) == Color(0, 0, 0));
    CHECK(img.get(0, 0) != Color(0, 0, 0));
}

void testRightClickPicksC() {
    MandelbrotRenderer m;
    Image img(100, 100);
    m.render(img);
    double re = 99;
    double im = 99;
    m.onPickC = [&](double r, double i) {
        re = r;
        im = i;
    };
    m.onClick(MouseButton::Right, 50, 50);
    CHECK_NEAR(re, -0.75, 1e-12);
    CHECK_NEAR(im, 0.0, 1e-12);
}

void testJuliaDependsOnC() {
    JuliaRenderer j;
    Image a(60, 40);
    Image b(60, 40);
    j.render(a);
    j.setC(0.4, 0.4); // outside the Mandelbrot set -> disconnected "dust"
    j.render(b);
    int different = 0;
    for (int y = 0; y < 40; ++y)
        for (int x = 0; x < 60; ++x)
            different += a.get(x, y) != b.get(x, y) ? 1 : 0;
    CHECK(different > 100);
}

} // namespace

int main() {
    testKnownMandelbrotPoints();
    testCardioidShortcutAgreesWithIteration();
    testSmoothCountIsContinuous();
    testViewMapping();
    testZoomKeepsPointUnderCursor();
    testPanFollowsCursor();
    testFitShowsWholeArea();
    testPaletteWrapsAround();
    testMandelbrotRender();
    testRightClickPicksC();
    testJuliaDependsOnC();
    return testResult();
}
