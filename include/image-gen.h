#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <sstream>

class ImageGen {
public:
    ImageGen();
    ImageGen(const sf::Image& image);
    void Loop();
    std::string ColorToString(const sf::Color color);
    size_t CalculateSumOfColorDifferences(sf::Image&);
    void DrawRandomCircle(sf::RenderTexture&);
private:
    sf::RenderWindow window_;
    sf::Vector2u windowResolution_;

    sf::Image sourceImage_;
    sf::Vector2u sourceImageSize_;
};