#pragma once

#include "core/Image.hpp"

#include <string>

// Zapisuje obraz do pliku; format wynika z rozszerzenia (.png albo .bmp).
// Rzuca std::runtime_error, gdy zapis sie nie powiedzie.
void saveImage(const Image& image, const std::string& path);
