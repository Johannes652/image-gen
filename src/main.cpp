#include <iostream>
#include "image-gen.h"

int main(int argc, char* argv[]) {
    sf::Image image;
    std::string imageFilePath;
    if (argc < 2) {
        // default behaviour
        imageFilePath = "resources/source-images/dannebrog.jpg";
    }
    else {
        imageFilePath = argv[1];
    }

    if (!image.loadFromFile(imageFilePath)) {
        std::cerr << "Could not load image from filepath " << imageFilePath << "\n";
        return 1;
    } 
    else std::cout << "Successfully loaded image!\n";
    
    ImageGen imageGen(image);
    imageGen.Loop();
    return 0;
}   