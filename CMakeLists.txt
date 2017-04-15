# Minimal version for CMake
cmake_minimum_required(VERSION 2.8)

# Project's name
project(irstd)

# Set default compilation flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_subdirectory(Test)
add_subdirectory(tests)

set(irstd_sources
	Exception/Exception.cpp
	Logger/Logger.cpp
	Memory/Memory.cpp
	Bootstrap/Bootstrap.cpp
)

add_library(irstd STATIC ${irstd_sources})
