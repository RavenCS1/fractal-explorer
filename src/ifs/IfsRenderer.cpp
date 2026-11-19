#include "ifs/IfsRenderer.hpp"
#include "core/LineReader.hpp"
#include <imgui.h>
#include <stdexcept>
#include <utility>
#include <fstream>
#include <iomanip>
#include <limits>
#include <algorithm>
#include <array>
#include <cmath>

namespace {

constexpr Color coral(255, 110, 110);
constexpr Color green(100, 220, 140);
constexpr Color blue(100, 170, 255);
constexpr Color gold(255, 210, 100);
constexpr Color violet(190, 130, 255);
constexpr Color cyan(100, 220, 220);

constexpr std::array<Color, 6> transformColors{{coral, green, blue, gold, violet, cyan}};

} // namespace

void IfsRenderer::render(Image& image) {
	image.fill(Color(0, 0, 0));
	if (transforms_.empty() || image.empty()) {
		return;
	}

	std::vector<double> weights;
	weights.reserve(transforms_.size());
	double maximumWeight{0.0};
	for (const auto& transform : transforms_) {
		weights.push_back(transform.p);
		maximumWeight = std::max(maximumWeight, transform.p);
	}
	// normalize weights to prevent an overflowing sum
	for (auto& weight : weights) {
		weight /= maximumWeight;
	}

	// chaos game
	std::discrete_distribution<std::size_t> chooseTransform(
			weights.begin(),
			weights.end()
		);
	Point point{};
	constexpr int burnIn{20};
	for (int i = 0; i < burnIn; ++i) {
		const auto index = chooseTransform(rng_);
		point = transforms_[index].apply(point);
		if (!std::isfinite(point.x) || !std::isfinite(point.y)) {
			return;
		}
	}
	double minX = point.x;
	double maxX = point.x;
	double minY = point.y;
	double maxY = point.y;
	constexpr int boundsSamples{5000};
	for (int i = 0; i < boundsSamples; ++i) {
		const auto index = chooseTransform(rng_);
		point = transforms_[index].apply(point);
		if (!std::isfinite(point.x) || !std::isfinite(point.y)) {
			return;
		}
		minX = std::min(minX, point.x);
		maxX = std::max(maxX, point.x);
		minY = std::min(minY, point.y);
		maxY = std::max(maxY, point.y);
	}

	const double spanX = maxX - minX;
	const double spanY = maxY - minY;
	if (!std::isfinite(spanX) || !std::isfinite(spanY)) {
		return;
	}
	const double centerX = minX + (spanX / 2.0);
	const double centerY = minY + (spanY / 2.0);

	// avoid division by zero
	constexpr double minimumSpan = 1e-12;
	const double scaleX = (image.width() - 1) / std::max(spanX, minimumSpan);
	const double scaleY = (image.height() - 1) / std::max(spanY, minimumSpan);
	const double scale = 0.9 * std::min(scaleX, scaleY);
	const double pixelCenterX = (image.width() - 1) / 2.0;
	const double pixelCenterY = (image.height() - 1) / 2.0;

	constexpr int pointCount{100000};
	for (int i = 0; i < pointCount; ++i) {
		const auto index = chooseTransform(rng_);
		point = transforms_[index].apply(point);
		if (!std::isfinite(point.x) || !std::isfinite(point.y)) {
			image.fill(Color(0, 0, 0));
			return;
		}

		const double px = pixelCenterX + ((point.x - centerX) * scale);
		const double py = pixelCenterY - ((point.y - centerY) * scale);
		if (std::isfinite(px) && std::isfinite(py) && px >= 0.0 && py >= 0.0 &&
			px <= image.width() - 1 && py <= image.height() - 1) {
			image.set(static_cast<int>(std::lround(px)), static_cast<int>(std::lround(py)),
					transformColors[index % transformColors.size()]);
		}
	}
}

bool IfsRenderer::drawControls() {
    ImGui::TextWrapped("Load an .ifs definition using File > Load definition.");
    ImGui::Text("Transforms: %zu", transforms_.size());
    ImGui::TextUnformatted("Chaos game: 100,000 points");
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
	if (transforms_.empty()) {
		throw std::runtime_error("No IFS definition to save");
	}

	std::ofstream out(path);
	if (!out) {
		throw std::runtime_error("Cannot open file for writing: " + path);
	}

	out << std::setprecision(std::numeric_limits<double>::max_digits10);
	for (const auto& transform : transforms_) {
		out << transform.a << ' '
			<< transform.b << ' '
			<< transform.c << ' '
			<< transform.d << ' '
			<< transform.e << ' '
			<< transform.f << ' '
			<< transform.p << '\n';
	}
	out.close();
	if (!out) {
		throw std::runtime_error("Failed to write definition: " + path);
	}
}
