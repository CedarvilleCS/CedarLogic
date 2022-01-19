cmake_minimum_required(VERSION 3.8)

project(Logic VERSION 0.0.0)

configure_file(Logic_version.hpp.in Logic_version.hpp)

file(GLOB SRC_FILES 
	"${PROJECT_SOURCE_DIR}/src/*.cpp"
	"${PROJECT_SOURCE_DIR}/include/*.h"
)


add_library(Logic STATIC "${SRC_FILES}")

# To build this library you must include it's headers
include_directories(	
	"${PROJECT_SOURCE_DIR}/include/"
)

# Make it so anyone using this library (and CMake) knows to include the library's headers.
target_include_directories(Logic PUBLIC "${PROJECT_SOURCE_DIR}/include")

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
