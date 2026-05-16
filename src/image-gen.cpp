#include "image-gen.h"
#include <optional>
#include <iostream>
#include <random>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <future>
#include <vector>
#include <numeric>

#define FRAMERATE 0

ImageGen::ImageGen(const sf::Image& image) :
    sourceImage_(image),
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

        // Calculate sum of differences of average colors
        size_t oldSumOfColorDifferences = CalculateSumOfColorDifferences(baseImage);

        // Place random shape
        DrawRandomCircle(canvas);
        canvas.display();
        
        // Calculate new sum of differences of average colors
        sf::Image mutatedImage = canvas.getTexture().copyToImage();
        size_t newSumOfColorDifferences = CalculateSumOfColorDifferences(mutatedImage);

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

size_t ImageGen::CalculateSumOfColorDifferences(sf::Image& image) {
    if (sourceImage_.getSize() != image.getSize()) {
        std::cout << "Images not equal size!";
        return 0;
    }

    size_t numberOfPixels = sourceImageSize_.x * sourceImageSize_.y;
    //size_t totalBytes = numberOfPixels * 4;

    const uint8_t* sourceImagePixels = sourceImage_.getPixelsPtr();
    const uint8_t* windowImagePixels = image.getPixelsPtr();

    size_t numThreads = std::thread::hardware_concurrency();
    if (numThreads == 0) numThreads = 4; // Fallback

    size_t pixelsPerThread = numberOfPixels / numThreads;
    std::vector<std::future<size_t>> futures;

    auto processChunk = [&](size_t startPixel, size_t endPixel) -> size_t {
        size_t localSum = 0;
        for (size_t i = startPixel * 4; i < endPixel; i += 4) {
            uint8_t rDiff = static_cast<uint8_t>(std::abs(sourceImagePixels[i] - windowImagePixels[i]));
            uint8_t gDiff = static_cast<uint8_t>(std::abs(sourceImagePixels[i+1] - windowImagePixels[i+1]));
            uint8_t bDiff = static_cast<uint8_t>(std::abs(sourceImagePixels[i+2] - windowImagePixels[i+2]));
            localSum += (rDiff + gDiff + bDiff) / 3;
        }
        return localSum;
    };
    
    // Dispatch threads
    for (size_t t = 0; t < numThreads; ++t) {
        size_t startPixel = t * pixelsPerThread;
        // Ensure the last thread processes any remaining pixels
        size_t endPixel = (t == numThreads - 1) ? numberOfPixels : startPixel + pixelsPerThread;
        
        futures.push_back(std::async(std::launch::async, processChunk, startPixel, endPixel));
    }

    // Wait for all threads to finish and accumulate the results
    size_t totalSumOfDifferences = 0;
    for (auto& f : futures) {
        totalSumOfDifferences += f.get();
    }

    return totalSumOfDifferences;
}

void ImageGen::DrawRandomCircle(sf::RenderTexture& canvas) {
    sf::CircleShape circle;
    // 1 <= radius <= min(windowsize)
    size_t maxRadius = std::min(windowResolution_.x, windowResolution_.y);
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distrib(1, maxRadius);

    int radius = distrib(gen);
    circle.setRadius(radius);

    // -2*radius <= xpos <= (windowwidth + 2*radius)
    // -2*radius <= ypos <= (windowheight + 2*radius)
    distrib = std::uniform_int_distribution<int>{-2*radius, static_cast<int>(windowResolution_.x) + 2*radius};
    float xpos = distrib(gen);
    distrib = std::uniform_int_distribution<int>{-2*radius, static_cast<int>(windowResolution_.y) + 2*radius};
    float ypos = distrib(gen);
    circle.setPosition({xpos, ypos});

    // 0 <= col <= 255
    distrib = std::uniform_int_distribution<int>{0, 255};
    circle.setFillColor({
        static_cast<uint8_t>(distrib(gen)),
        static_cast<uint8_t>(distrib(gen)),
        static_cast<uint8_t>(distrib(gen)),
        static_cast<uint8_t>(distrib(gen))
    });

    canvas.draw(circle);
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