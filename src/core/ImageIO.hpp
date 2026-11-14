#pragma once

#include "core/Image.hpp"

#include <string>

// Saves the image to a file; format is determined by extension (.png or .bmp).
// Throws std::runtime_error on failure.
void saveImage(const Image& image, const std::string& path);
