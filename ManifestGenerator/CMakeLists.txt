cmake_minimum_required(VERSION 3.20)
project(ManifestGenerator)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(ManifestGenerator src/main.cpp
        src/main.h)
