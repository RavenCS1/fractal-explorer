#include "TestUtil.hpp"
#include "core/LineReader.hpp"
#include "lsystem/LSystem.hpp"

#include <algorithm>
#include <cstddef>
#include <string>

static LSystem parseText(const std::string& text) {
    LineReader in = LineReader::fromString(text);
    return LSystem::parse(in);
}

// --- valid files ---

static void testParseKoch() {
    const LSystem s = parseText("axiom F\n"
                                "rule F=F+F-F-F+F\n"
                                "angle 90\n"
                                "iterations 4\n");
    CHECK(s.axiom == "F");
    CHECK(s.rules.size() == 1);
    CHECK(s.rules.count('F') == 1 && s.rules.at('F') == "F+F-F-F+F");
    CHECK_NEAR(s.angle, 90.0, 1e-12);
    CHECK(s.iterations == 4);
}

static void testParseCommentsBlankLinesAndOrder() {
    const LSystem s = parseText("# Koch curve\n"
                                "\n"
                                "iterations 3   # keywords in any order\n"
                                "\r\n"
                                "angle 90\r\n"
                                "axiom F\n"
                                "rule F=F+F-F-F+F\n");
    CHECK(s.axiom == "F");
    CHECK(s.rules.at('F') == "F+F-F-F+F");
    CHECK_NEAR(s.angle, 90.0, 1e-12);
    CHECK(s.iterations == 3);
}

static void testParseSpacesAndTabs() {
    const LSystem s = parseText("axiom\tF + F\n"
                                "rule F = F + F - F\n"
                                "angle   +22.5\n"
                                "iterations\t+2\n");
    CHECK(s.axiom == "F+F");
    CHECK(s.rules.at('F') == "F+F-F");
    CHECK_NEAR(s.angle, 22.5, 1e-12);
    CHECK(s.iterations == 2);
}

static void testParseSeveralRules() {
    // Heighway dragon
    const LSystem s = parseText("axiom FX\n"
                                "rule X=X+YF+\n"
                                "rule Y=-FX-Y\n"
                                "angle 90\n"
                                "iterations 10\n");
    CHECK(s.axiom == "FX");
    CHECK(s.rules.size() == 2);
    CHECK(s.rules.at('X') == "X+YF+");
    CHECK(s.rules.at('Y') == "-FX-Y");
}

static void testParseEdgeValues() {
    const LSystem s = parseText("axiom X\n"
                                "rule X=\n" // empty right side: "delete X"
                                "angle -60\n"
                                "iterations 0\n");
    CHECK(s.rules.at('X').empty());
    CHECK_NEAR(s.angle, -60.0, 1e-12);
    CHECK(s.iterations == 0);

    const LSystem t = parseText("axiom F\nangle 1e1\niterations 15\n"); // no rules is fine
    CHECK(t.rules.empty());
    CHECK_NEAR(t.angle, 10.0, 1e-12);
    CHECK(t.iterations == LSystem::kMaxIterations);
}

// --- errors with a line number ---

static void testErrorsInRules() {
    CHECK_PARSE_ERROR_LINE(parseText("axiom F\nrule F\nangle 90\niterations 2\n"), 2);    // missing '='
    CHECK_PARSE_ERROR_LINE(parseText("axiom F\nrule FF=F\nangle 90\niterations 2\n"), 2); // two symbols on the left
    CHECK_PARSE_ERROR_LINE(parseText("axiom F\nrule =F\nangle 90\niterations 2\n"), 2);   // no symbol on the left
    CHECK_PARSE_ERROR_LINE(parseText("axiom F\nrule F=F\nrule F=FF\nangle 90\niterations 2\n"), 3); // duplicate rule
}

static void testErrorsInNumbers() {
    CHECK_PARSE_ERROR_LINE(parseText("axiom F\nangle abc\niterations 2\n"), 2);
    CHECK_PARSE_ERROR_LINE(parseText("axiom F\nangle 90deg\niterations 2\n"), 2);
    CHECK_PARSE_ERROR_LINE(parseText("axiom F\nangle inf\niterations 2\n"), 2);
    CHECK_PARSE_ERROR_LINE(parseText("axiom F\nangle\niterations 2\n"), 2); // no value
    CHECK_PARSE_ERROR_LINE(parseText("axiom F\nangle 90\niterations 4.5\n"), 3);
    CHECK_PARSE_ERROR_LINE(parseText("axiom F\nangle 90\niterations -1\n"), 3);
    CHECK_PARSE_ERROR_LINE(parseText("axiom F\nangle 90\niterations 16\n"), 3);
}

static void testErrorsInKeywords() {
    CHECK_PARSE_ERROR_LINE(parseText("axiom F\nfoo 3\n"), 2);                                // unknown keyword
    CHECK_PARSE_ERROR_LINE(parseText("Axiom F\nangle 90\niterations 2\n"), 1);               // lowercase only
    CHECK_PARSE_ERROR_LINE(parseText("axiom\nangle 90\niterations 2\n"), 1);                 // empty axiom
    CHECK_PARSE_ERROR_LINE(parseText("axiom F\naxiom G\nangle 90\niterations 2\n"), 2);      // duplicate axiom
    CHECK_PARSE_ERROR_LINE(parseText("axiom F\nangle 90\nangle 60\niterations 2\n"), 3);     // duplicate angle
    CHECK_PARSE_ERROR_LINE(parseText("axiom F\niterations 2\nangle 90\niterations 3\n"), 4); // duplicate iterations
}

static void testErrorLineCountsCommentsAndBlankLines() {
    CHECK_PARSE_ERROR_LINE(parseText("# header\n"
                                     "\n"
                                     "axiom F\n"
                                     "# comment\n"
                                     "angle oops\n"
                                     "iterations 2\n"),
                           5);
}

// --- file-level errors (line 0) ---

static void testMissingSections() {
    CHECK_PARSE_ERROR_LINE(parseText(""), 0);
    CHECK_PARSE_ERROR_LINE(parseText("# only a comment\n"), 0);
    CHECK_PARSE_ERROR_LINE(parseText("rule F=F\nangle 90\niterations 2\n"), 0); // no axiom
    CHECK_PARSE_ERROR_LINE(parseText("axiom F\niterations 2\n"), 0);            // no angle
    CHECK_PARSE_ERROR_LINE(parseText("axiom F\nangle 90\n"), 0);                // no iterations
}

// --- expand() ---

static LSystem makeKoch(int iterations) {
    LSystem s;
    s.axiom = "F";
    s.rules['F'] = "F+F-F-F+F";
    s.iterations = iterations;
    return s;
}

static std::size_t pow5(int n) {
    std::size_t r = 1;
    for (int i = 0; i < n; ++i)
        r *= 5;
    return r;
}

static void testExpandKochSmall() {
    const LSystem::Expansion e0 = makeKoch(0).expand();
    CHECK(e0.text == "F");
    CHECK(e0.iterationsDone == 0);
    CHECK(!e0.limitReached);

    const LSystem::Expansion e1 = makeKoch(1).expand();
    CHECK(e1.text == "F+F-F-F+F");
    CHECK(e1.iterationsDone == 1);
    CHECK(!e1.limitReached);
}

static void testExpandKochGrowth() {
    // Each F becomes 5 F's and 4 signs: F count = 5^n, length = 2 * 5^n - 1.
    for (int n = 0; n <= 6; ++n) {
        const LSystem::Expansion e = makeKoch(n).expand();
        const auto fCount = static_cast<std::size_t>(std::count(e.text.begin(), e.text.end(), 'F'));
        CHECK(fCount == pow5(n));
        CHECK(e.text.size() == 2 * pow5(n) - 1);
        CHECK(e.iterationsDone == n);
    }
}

static void testExpandIsSimultaneous() {
    LSystem s;
    s.axiom = "AB";
    s.rules['A'] = "B";
    s.rules['B'] = "A";
    s.iterations = 1;
    CHECK(s.expand().text == "BA"); // not "AA" or "BB"
}

static void testExpandAlgaeFibonacci() {
    LSystem s;
    s.axiom = "A";
    s.rules['A'] = "AB";
    s.rules['B'] = "A";
    const std::size_t expected[] = {1, 2, 3, 5, 8, 13};
    for (int n = 0; n < 6; ++n) {
        s.iterations = n;
        CHECK(s.expand().text.size() == expected[n]);
    }
    s.iterations = 4;
    CHECK(s.expand().text == "ABAABABA");
}

static void testExpandEmptyRuleAndNoRules() {
    LSystem s;
    s.axiom = "FXF";
    s.rules['X'] = ""; // X disappears
    s.iterations = 1;
    CHECK(s.expand().text == "FF");

    LSystem t;
    t.axiom = "F+[F]-f";
    t.iterations = 3; // no rules: unchanged
    const LSystem::Expansion e = t.expand();
    CHECK(e.text == "F+[F]-f");
    CHECK(e.iterationsDone == 3);
    CHECK(!e.limitReached);
}

static void testExpandStopsAtLimit() {
    // Default limit 5M: iteration 10 would be ~19.5M, so stop after 9 (2 * 5^9 - 1 = 3'906'249).
    const LSystem::Expansion e = makeKoch(10).expand();
    CHECK(e.limitReached);
    CHECK(e.iterationsDone == 9);
    CHECK(e.text.size() == 2 * pow5(9) - 1);

    // Small limit: lengths 1, 9, 49, 249 -> 249 > 100, so stop after 2 iterations.
    const LSystem::Expansion small = makeKoch(5).expand(100);
    CHECK(small.limitReached);
    CHECK(small.iterationsDone == 2);
    CHECK(small.text.size() == 49);

    // Exactly at the limit is still allowed.
    const LSystem::Expansion exact = makeKoch(2).expand(49);
    CHECK(!exact.limitReached);
    CHECK(exact.iterationsDone == 2);
}

int main() {
    testParseKoch();
    testParseCommentsBlankLinesAndOrder();
    testParseSpacesAndTabs();
    testParseSeveralRules();
    testParseEdgeValues();
    testErrorsInRules();
    testErrorsInNumbers();
    testErrorsInKeywords();
    testErrorLineCountsCommentsAndBlankLines();
    testMissingSections();
    testExpandKochSmall();
    testExpandKochGrowth();
    testExpandIsSimultaneous();
    testExpandAlgaeFibonacci();
    testExpandEmptyRuleAndNoRules();
    testExpandStopsAtLimit();
    return testResult();
}
