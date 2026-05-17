#include <iostream>
#include "image-gen.h"
#include <filesystem>

int main(int argc, char* argv[]) {
    sf::Image image;
    std::string imageFilePath;
    std::string shapeType = "circle";

    std::filesystem::path exePath = std::filesystem::weakly_canonical(std::filesystem::path(argv[0])).parent_path();

    if (argc < 2) {
        // default behaviour
        std::filesystem::path defaultImgPath = exePath / "../resources/source-images/dannebrog.jpg";
        imageFilePath = defaultImgPath.string();
    }
    else {
        if (argc > 2) {
            shapeType = argv[2];
        }
        imageFilePath = argv[1];
    }

    if (!image.loadFromFile(imageFilePath)) {
        std::cerr << "Could not load image from filepath " << imageFilePath << "\n";
        return 1;
    } 
    else std::cout << "Successfully loaded image!\n";
    
    ImageGen imageGen(image, shapeType);
    std::filesystem::path windowIconPath = exePath / "../resources/icon/icon.png";
    imageGen.SetWindowIcon(windowIconPath.string());
    imageGen.Loop();
    return 0;
}   