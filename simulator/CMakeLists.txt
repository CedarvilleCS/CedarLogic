cmake_minimum_required(VERSION 3.11)

project(Simulator VERSION 0.0.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(Simulator STATIC 
  "${PROJECT_SOURCE_DIR}/circuit.cpp"
  "${PROJECT_SOURCE_DIR}/circuit.hpp"
  "${PROJECT_SOURCE_DIR}/event.cpp"
  "${PROJECT_SOURCE_DIR}/event.hpp"
  "${PROJECT_SOURCE_DIR}/gate.hpp"
  "${PROJECT_SOURCE_DIR}/junction.hpp"
  "${PROJECT_SOURCE_DIR}/logic.cpp"
  "${PROJECT_SOURCE_DIR}/logic.hpp"
  "${PROJECT_SOURCE_DIR}/network.hpp"
  "${PROJECT_SOURCE_DIR}/wire.hpp"
)

# To build this library you must include it's headers
include_directories(	
	"${PROJECT_SOURCE_DIR}/"
)

# Make it so anyone using this library (and CMake) knows to include the library's headers.
target_include_directories(Simulator PUBLIC "${PROJECT_SOURCE_DIR}/")

# Bring in the Catch2 framework
Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.0.0-preview4
)

FetchContent_MakeAvailable(Catch2)

# Add a executable to run the tests
add_executable(test_Simulator 
	tests/test_logic.cpp
)

# Add the libraries the test executable will need to run
target_link_libraries(test_Simulator PRIVATE Catch2::Catch2WithMain Simulator)
