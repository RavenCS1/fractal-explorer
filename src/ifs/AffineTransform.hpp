#pragma once

#include "ifs/Point.hpp"

struct AffineTransform {
	double a;
	double b;
	double c;
	double d;
	double e;
	double f;
	double p;

	[[nodiscard]] Point apply(const Point& point) const {
		return {(a * point.x) + (b * point.y) + e,
			    (c * point.x) + (d * point.y) + f};
	}
};
