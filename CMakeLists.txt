cmake_minimum_required(VERSION 3.29)
project(OpenCV)

set(CMAKE_CXX_STANDARD 20)

# Find OpenCV package
set(OpenCV_DIR "/home/bakr/Downloads/opencv-4.10.0/build") # Make sure this points to the correct build directory
find_package(OpenCV REQUIRED)

# Include directories for OpenCV
include_directories(${OpenCV_INCLUDE_DIRS})

# Link OpenCV libraries
add_executable(OpenCV "main.cpp"
        main.cpp
)
target_link_libraries(OpenCV ${OpenCV_LIBS})
