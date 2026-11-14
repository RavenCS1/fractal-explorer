#include "core/ImageIO.hpp"

#include <SFML/Graphics/Image.hpp>

#include <stdexcept>

void saveImage(const Image& image, const std::string& path) {
    if (image.empty())
        throw std::runtime_error("Brak obrazu do zapisania");

    const sf::Image out(sf::Vector2u(static_cast<unsigned>(image.width()),
                                     static_cast<unsigned>(image.height())),
                        image.bytes());
    if (!out.saveToFile(path))
        throw std::runtime_error("Nie udalo sie zapisac pliku: " + path);
}
