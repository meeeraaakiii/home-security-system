cmake_minimum_required(VERSION 3.10)
project(ShowStream LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")

include_directories(${CMAKE_SOURCE_DIR}/src/include)

# Use GLOB_RECURSE to find all .cpp files, including module files src/.../*.cpp
file(GLOB_RECURSE SOURCES "src/cmd/*.cpp" "src/pkg/*.cpp")

# message("SOURCES BEFORE: ${SOURCES}")
# # Exclude the main file from the matrix mult SOURCES list
# set(MAIN_FILE "src/main.cpp")
# list(FILTER SOURCES EXCLUDE REGEX ${MAIN_FILE})
message("SOURCES: ${SOURCES}")

add_executable(ShowStream ${SOURCES} ${MAIN_FILE})
# Include directories for headers
target_include_directories(ShowStream PRIVATE include)
