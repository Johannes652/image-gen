#include "image-gen.h"
#include <optional>
#include <iostream>
#include <random>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <algorithm>

#define FRAMERATE 0

ImageGen::ImageGen(const sf::Image& image, const std::string& shapeType) :
    sourceImage_(image),
    shapeType_(shapeType),
    sourceImageSize_(image.getSize()),
    windowResolution_(image.getSize())
{
    window_.create(sf::VideoMode(windowResolution_), "Image Gen", sf::Style::Titlebar | sf::Style::Close);
    window_.setFramerateLimit(FRAMERATE);
}

void ImageGen::Loop() {
    sf::RenderTexture canvas(windowResolution_);
    canvas.clear(sf::Color::Black);
    canvas.display();

    while (window_.isOpen()) {
        
        while(const std::optional event = window_.pollEvent()) {
            
            if (event->is<sf::Event::Closed>()) {
                window_.close();
                return;
            }
            else if (const auto* keyEvent = event->getIf<sf::Event::KeyPressed>()) {
                if (keyEvent->code == sf::Keyboard::Key::C && keyEvent->control) {
                    std::cout << "Ctrl+C detected. Saving image...\n";
                    sf::Image finalImage = canvas.getTexture().copyToImage();

                    SaveEvolvedImage(finalImage);

                    window_.close();
                    return;
                }
            }
        }
        // Convert window pixels to sf::image
        sf::Image baseImage = canvas.getTexture().copyToImage();

        std::pair<sf::Vector2u, sf::Vector2u> coords;
        // Place random shape
        if (shapeType_ == "rectangle" || shapeType_ == "rect") {
            coords = DrawRandomRect(canvas);
        }
        else {
            // defaults to circle shapes
            coords = DrawRandomCircle(canvas);
        }

        // Calculate sum of differences of average colors of baseImage
        size_t oldSumOfColorDifferences = CalculateSumOfColorDifferences(baseImage, coords);
        
        canvas.display();
        
        // Calculate new sum of differences of average colors
        sf::Image mutatedImage = canvas.getTexture().copyToImage();
        size_t newSumOfColorDifferences = CalculateSumOfColorDifferences(mutatedImage, coords);

        // Determine if new diff is better than old diff
        if (newSumOfColorDifferences >= oldSumOfColorDifferences) {
            std::cout << "Old iteration was better!\n";
            //Old was better, draw old again from saved image
            sf::Texture baseTexture;
            if (baseTexture.loadFromImage(baseImage)) {
                sf::Sprite restoreSprite(baseTexture);
                canvas.draw(restoreSprite);
                canvas.display();
            }
        }
        else {
            std::cout << "New iteration is better!\n";
        }

        // Draw to window
        window_.clear();
        sf::Sprite windowSprite(canvas.getTexture());
        window_.draw(windowSprite);
        window_.display();
    }
}

size_t ImageGen::CalculateSumOfColorDifferences(sf::Image& image, std::pair<sf::Vector2u, sf::Vector2u>& coords) {
    if (sourceImage_.getSize() != image.getSize()) {
        std::cout << "Images not equal size!";
        return 0;
    }

    auto startCoords = coords.first;
    auto endCoords = coords.second;

    size_t sumOfColorDifferences {0};
    size_t numberOfPixels = (endCoords.x - startCoords.x) * (endCoords.y - startCoords.y);

    const uint8_t* sourceImagePixels = sourceImage_.getPixelsPtr();
    const uint8_t* windowImagePixels = image.getPixelsPtr();

    size_t spliceWidthBytes = (endCoords.x - startCoords.x) * 4;
    size_t imageWidth = sourceImageSize_.x;

    uint8_t redDifference{0}, greenDifference{0}, blueDifference{0};

    // Iterate row by row through the splice
    for (size_t y = startCoords.y; y < endCoords.y; ++y) {
        // Calculate the exact 1D byte index where this specific row starts in the image array
        size_t rowStartIndex = (y * imageWidth + startCoords.x) * 4;
        size_t rowEndIndex = rowStartIndex + spliceWidthBytes;

        // Now iterate through the pixels of the row
        for (size_t x = rowStartIndex; x < rowEndIndex; x += 4) {
            redDifference =     static_cast<uint8_t>(std::abs(sourceImagePixels[x]   - windowImagePixels[x]));
            greenDifference =   static_cast<uint8_t>(std::abs(sourceImagePixels[x+1] - windowImagePixels[x+1]));
            blueDifference =    static_cast<uint8_t>(std::abs(sourceImagePixels[x+2] - windowImagePixels[x+2]));

            sumOfColorDifferences += (redDifference + greenDifference + blueDifference);
        }
    }

    return sumOfColorDifferences;
}

std::pair<sf::Vector2u, sf::Vector2u> ImageGen::DrawRandomCircle(sf::RenderTexture& canvas) {
    sf::CircleShape circle;

    static std::random_device rd;
    static std::mt19937 gen(rd());

    // SIZE
    // 1 <= radius <= min(windowsize) / 4
    static int maxRadius = std::min(windowResolution_.x, windowResolution_.y) >> 2;
    static std::uniform_int_distribution<int> sizeDistrib(1, maxRadius);
    int radius = sizeDistrib(gen);
    circle.setRadius(radius);

    // POSITION
    // -2*radius <= xpos <= (windowwidth + 2*radius)
    int minPos = -2*radius;
    int maxXPos = windowResolution_.x + 2*radius;
    float xpos = std::uniform_int_distribution<int>{minPos, maxXPos}(gen);
    // -2*radius <= ypos <= (windowheight + 2*radius)
    int maxYPos = windowResolution_.y + 2*radius;
    float ypos = std::uniform_int_distribution<int>{minPos, maxYPos}(gen);
    circle.setPosition({xpos, ypos});

    // COLOR
    // 0 <= col <= 255
    static std::uniform_int_distribution<int> colorDistrib(0, 255);
    circle.setFillColor({
        static_cast<uint8_t>(colorDistrib(gen)),
        static_cast<uint8_t>(colorDistrib(gen)),
        static_cast<uint8_t>(colorDistrib(gen)),
        static_cast<uint8_t>(colorDistrib(gen))
    });

    canvas.draw(circle);

    sf::Vector2u pos1 {
        static_cast<unsigned int>( std::clamp(static_cast<int>(xpos), 0, static_cast<int>(windowResolution_.x)) ), 
        static_cast<unsigned int>( std::clamp(static_cast<int>(ypos), 0, static_cast<int>(windowResolution_.y)) )
    };
    sf::Vector2u pos2 {
        static_cast<unsigned int>( std::clamp(static_cast<int>(xpos) + (radius * 2), 0, static_cast<int>(windowResolution_.x)) ), 
        static_cast<unsigned int>( std::clamp(static_cast<int>(ypos) + (radius * 2), 0, static_cast<int>(windowResolution_.y)) )
    };
    return {pos1, pos2};
}

std::pair<sf::Vector2u, sf::Vector2u> ImageGen::DrawRandomRect(sf::RenderTexture& canvas) {
    sf::RectangleShape rect;

    static std::random_device rd;
    static std::mt19937 gen(rd());

    // SIZE
    // 1 <= xsize <= windowx / 4
    static int maxXSize = windowResolution_.x >> 2;
    float xsize = std::uniform_int_distribution<int>{1, maxXSize}(gen);
    // 1 <= ysize <= windowy / 4
    static int maxYSize = windowResolution_.x >> 2;
    float ysize = std::uniform_int_distribution<int>{1, maxYSize}(gen);
    rect.setSize({xsize,ysize});

    // POSITION
    // -sizex <= xpos <= windowx
    int minXPos = -xsize;
    static int maxXPos = windowResolution_.x;
    float xpos = std::uniform_int_distribution<int>{minXPos, maxXPos}(gen);
    // -sizey <= ypos <= windowy
    int minYPos = -ysize;
    static int maxYPos = windowResolution_.y;
    float ypos = std::uniform_int_distribution<int>{minYPos, maxYPos}(gen);
    rect.setPosition({xpos,ypos});

    // COLOR
    // 0 <= col <= 255
    static std::uniform_int_distribution<int> colorDistrib(0, 255);
    rect.setFillColor({
        static_cast<uint8_t>(colorDistrib(gen)),
        static_cast<uint8_t>(colorDistrib(gen)),
        static_cast<uint8_t>(colorDistrib(gen)),
        static_cast<uint8_t>(colorDistrib(gen))
    });

    canvas.draw(rect);

    sf::Vector2u pos1 {
        static_cast<unsigned int>( std::clamp(static_cast<int>(xpos), 0, static_cast<int>(windowResolution_.x)) ), 
        static_cast<unsigned int>( std::clamp(static_cast<int>(ypos), 0, static_cast<int>(windowResolution_.y)) )
    };
    sf::Vector2u pos2 {
        static_cast<unsigned int>( std::clamp(static_cast<int>(xpos + xsize), 0, static_cast<int>(windowResolution_.x)) ), 
        static_cast<unsigned int>( std::clamp(static_cast<int>(ypos + ysize), 0, static_cast<int>(windowResolution_.y)) )
    };
    return {pos1, pos2};
}

std::string ImageGen::ColorToString(const sf::Color color) {
    std::stringstream str;
    str << "R:"  << static_cast<int>(color.r) 
        << " G:" << static_cast<int>(color.g) 
        << " B:" << static_cast<int>(color.b);
    return str.str();
}

void ImageGen::SaveEvolvedImage(const sf::Image& image) const {
    namespace fs = std::filesystem;

    // Define the output directory relative to where the executable is run
    fs::path outputDir = "outputs";

    // Create the directory if it doesn't exist
    if (!fs::exists(outputDir)) {
        fs::create_directory(outputDir);
    }

    // Generate a timestamp string (e.g., "20260516_214002")
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S");
    
    // Construct the final file path
    std::string filename = "evolution_" + ss.str() + ".png";
    fs::path filePath = outputDir / filename;

    // Save the image
    if (image.saveToFile(filePath.string())) {
        std::cout << "Successfully saved image to: " << filePath.string() << "\n";
    } else {
        std::cerr << "Failed to save image.\n";
    }
}