#pragma once

/**
 * @file
 * Saving images to disk.
 */

#include "core/Image.hpp"

#include <string>

/**
 * Saves the image to a file; the format is determined by the extension (.png or .bmp).
 * @throws std::runtime_error on failure.
 */
void saveImage(const Image& image, const std::string& path);
