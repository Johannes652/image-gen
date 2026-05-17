# Image Evolver

This project is a C++ application that attempts to recreate a target image using randomly generated circles through an evolutionary algorithm. It uses SFML for rendering and image processing.

## Requirements

* C++17 compatible compiler (or higher)
* CMake 
* SFML 3.1.0

## Building the Project

1. Open a terminal in the root directory of the project.
2. Create a build directory and navigate into it:
   ```bash
   mkdir build
   cd build
4. Run CMake to configure the project:
   ```bash
   cmake ..
6. Build the executable:
   ```bash
   cmake --build .

## Usage

The program runs from the command line and requires the file path of the target image to be passed as an argument.
Navigate to your build directory where the executable was generated and run the program:
Note: <shape-type> is "rectangle", "rect" or "circle". Defaults to "circle".
### Windows
```bash
image-gen.exe "path/to/your/image.jpg" <shape-type>
```
### Linux / macOS:
```bash
./image-gen "path/to/your/image.jpg" <shape-type>
```
## How it Works

The program loads the target image provided via the command line.

It initializes a blank canvas.

In a continuous loop, it generates a random circle with a random radius, position, and RGB color.

It compares the newly generated canvas against the target image by calculating the absolute difference in pixel colors.

If the mutation (the new circle) results in a closer match to the target image, the mutation is kept. If it is worse, the mutation is rejected and the canvas reverts to the previous state.

The window continuously updates to display the current best approximation.

Press ctrl + C in the program window to save current iteration to image and exit the program.
