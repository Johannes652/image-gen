#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <sstream>

class ImageGen {
public:
    ImageGen(const sf::Image& image);
    void Loop();
    
private:
    sf::RenderWindow window_;
    sf::Vector2u windowResolution_;

    sf::Image sourceImage_;
    sf::Vector2u sourceImageSize_;
    
    std::string ColorToString(const sf::Color color);
    size_t CalculateSumOfColorDifferences(sf::Image&);
    void DrawRandomCircle(sf::RenderTexture&);
    void SaveEvolvedImage(const sf::Image& image) const;
};