#include "TestUtil.hpp"
#include "core/Image.hpp"
#include "core/LineReader.hpp"

static void testLineReaderSkipsCommentsAndCountsLines() {
    LineReader in = LineReader::fromString("# comment\n"
                                           "\n"
                                           "3   # number of transforms\n"
                                           "0.5 0 0 0.5 0 0 0.33\r\n"
                                           "0.5,0,0,0.5,0.5,0,0.33\n");

    CHECK(in.next());
    CHECK(in.lineNumber() == 3);
    CHECK(in.integer() == 3);

    CHECK(in.next());
    CHECK(in.lineNumber() == 4);
    const auto a = in.numbers(7);
    CHECK_NEAR(a[0], 0.5, 1e-12);
    CHECK_NEAR(a[6], 0.33, 1e-12);

    CHECK(in.next());
    const auto b = in.numbers(7); // commas are also separators
    CHECK_NEAR(b[4], 0.5, 1e-12);

    CHECK(!in.next());
}

static void testLineReaderErrorsHaveLineNumbers() {
    LineReader in = LineReader::fromString("1 2 3\n\n1 2 abc\n1.5\n");
    CHECK(in.next());
    CHECK_PARSE_ERROR_LINE(in.numbers(4), 1); // wrong number of values
    CHECK(in.next());
    CHECK_PARSE_ERROR_LINE(in.numbers(), 3); // invalid number
    CHECK(in.next());
    CHECK_PARSE_ERROR_LINE(in.integer(), 4); // not an integer
}

static void testLineReaderMissingFile() {
    CHECK_PARSE_ERROR_LINE(LineReader::fromFile("no_such_file.ifs"), 0);
}

static void testLineReaderSignsAndExponents() {
    LineReader in = LineReader::fromString("-0.15 +0.28 1e-3\n");
    CHECK(in.next());
    const auto v = in.numbers(3);
    CHECK_NEAR(v[0], -0.15, 1e-12);
    CHECK_NEAR(v[1], 0.28, 1e-12);
    CHECK_NEAR(v[2], 0.001, 1e-12);
}

static void testImageBasics() {
    Image img(4, 3, Color(1, 2, 3));
    CHECK(img.width() == 4 && img.height() == 3);
    CHECK(img.get(3, 2) == Color(1, 2, 3));

    img.set(-1, 0, Color(255, 0, 0)); // outside image — no effect, no crash
    img.set(4, 0, Color(255, 0, 0));
    img.set(2, 1, Color(9, 9, 9));
    CHECK(img.get(2, 1) == Color(9, 9, 9));
    CHECK(img.bytes()[(1 * 4 + 2) * 4] == 9); // RGBA layout, row by row
}

static void testDrawLine() {
    Image img(10, 10);
    const Color white(255, 255, 255);
    img.drawLine(0, 0, 9, 9, white);
    for (int i = 0; i < 10; ++i)
        CHECK(img.get(i, i) == white);

    Image img2(10, 10);
    img2.drawLine(-50, 5, 50, 5, white); // clipped to image
    for (int x = 0; x < 10; ++x)
        CHECK(img2.get(x, 5) == white);
}

int main() {
    testLineReaderSkipsCommentsAndCountsLines();
    testLineReaderErrorsHaveLineNumbers();
    testLineReaderMissingFile();
    testLineReaderSignsAndExponents();
    testImageBasics();
    testDrawLine();
    return testResult();
}
