#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <sstream>

class ImageGen {
public:
    ImageGen(const sf::Image& image, const std::string& shapeType);
    void SetWindowIcon(std::string filePath);
    void Loop();
    
private:
    std::string shapeType_;

    sf::RenderWindow window_;
    sf::Vector2u windowResolution_;

    sf::Image sourceImage_;
    sf::Vector2u sourceImageSize_;

    std::string ColorToString(const sf::Color color);
    size_t CalculateSumOfColorDifferences(sf::Image&, std::pair<sf::Vector2u, sf::Vector2u>& coords);
    std::pair<sf::Vector2u, sf::Vector2u> DrawRandomCircle(sf::RenderTexture&);
    std::pair<sf::Vector2u, sf::Vector2u> DrawRandomRect(sf::RenderTexture&);
    void SaveEvolvedImage(const sf::Image& image) const;
};