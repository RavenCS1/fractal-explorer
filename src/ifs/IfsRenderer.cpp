#include "ifs/IfsRenderer.hpp"
#include "core/LineReader.hpp"
#include <imgui.h>
#include <stdexcept>
#include <utility>

void IfsRenderer::render(Image& image) {
    // TODO: chaos game, color by transform, automatic scale fitting.
    for (int y = 0; y < image.height(); ++y)
        for (int x = 0; x < image.width(); ++x)
            image.set(x, y, Color(static_cast<std::uint8_t>(x), 160, static_cast<std::uint8_t>(y)));
}

bool IfsRenderer::drawControls() {
    ImGui::TextWrapped("TODO: built-in fractals, coefficient table a b c d e f p, transition animation.");
    return false;
}

void IfsRenderer::loadDefinition(const std::string& path) {
    LineReader in = LineReader::fromFile(path);
	std::vector<AffineTransform> loaded;
	bool hasPositiveProbability{false};

	while (in.next()) {
		auto values = in.numbers(7);
		if (values[6] < 0) {
			in.fail("Parameter p (probability) must not be negative");
		}
		if (values[6] > 0) {
			hasPositiveProbability = true;
		}
		loaded.push_back({values[0],
				          values[1],
						  values[2],
						  values[3],
						  values[4],
						  values[5],
						  values[6]
						});
	}
	if (loaded.empty()) {
		in.fail("Definition must contain at least one transformation");
	}
	if (!hasPositiveProbability) {
		in.fail("At least one probability must be positive");
	}

	transforms_ = std::move(loaded);
}

void IfsRenderer::saveDefinition(const std::string& path) const {
    (void)path;
    throw std::runtime_error("Saving .ifs is not implemented yet");
}
