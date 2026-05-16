#include <iostream>
#include "image-gen.h"

int main(int argc, char* argv[]) {
    sf::Image image;
    std::string imageFilePath;
    std::string shapeType = "circle";

    if (argc < 3) {
        // default behaviour
        imageFilePath = "resources/source-images/dannebrog.jpg";
    }
    else {
        imageFilePath = argv[1];
        shapeType = argv[2];
    }

    if (!image.loadFromFile(imageFilePath)) {
        std::cerr << "Could not load image from filepath " << imageFilePath << "\n";
        return 1;
    } 
    else std::cout << "Successfully loaded image!\n";
    
    ImageGen imageGen(image, shapeType);
    imageGen.Loop();
    return 0;
}   