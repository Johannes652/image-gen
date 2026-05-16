# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.14)

# Name project and specify the language (C++)
project(image-gen VERSION 1.0 LANGUAGES CXX)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# --- SFML Setup ---
include(FetchContent)

# Force static linking. This bundles SFML directly into .exe 
# so we don't have to copy missing .dll files next to the game later!
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static libraries" FORCE)

# Download SFML 3.0.2 directly from GitHub
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.1.0 
)
FetchContent_MakeAvailable(SFML)
# ------------------

# Create the executable using subdirectories
file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(image-gen ${SOURCES})

# Tell CMake where to look for header files
file(GLOB INCLUDE_SUBDIRS "${CMAKE_CURRENT_SOURCE_DIR}/include/*")
target_include_directories(image-gen PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link the SFML modules the project needs to run
target_link_libraries(image-gen PRIVATE sfml-graphics sfml-window sfml-system)