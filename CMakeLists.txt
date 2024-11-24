
cmake_minimum_required(VERSION 3.10)

project(Cloth_Simulation)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(Cloth_Simulation
        constraint.h
        input_handler.h
        main.cpp
        particle.h
)

list(APPEND CMAKE_PREFIX_PATH "/path/to/sfml")

find_package(SFML COMPONENTS system audio graphics REQUIRED)

target_link_libraries(Cloth_Simulation sfml-system sfml-audio sfml-graphics)
