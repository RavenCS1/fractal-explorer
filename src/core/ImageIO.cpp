#include "core/ImageIO.hpp"

#include <SFML/Graphics/Image.hpp>

#include <stdexcept>

void saveImage(const Image& image, const std::string& path) {
    if (image.empty())
        throw std::runtime_error("No image to save");

    const sf::Image out(sf::Vector2u(static_cast<unsigned>(image.width()), static_cast<unsigned>(image.height())),
                        image.bytes());
    if (!out.saveToFile(path))
        throw std::runtime_error("Failed to save file: " + path);
}
