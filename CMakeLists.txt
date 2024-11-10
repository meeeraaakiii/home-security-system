cmake_minimum_required(VERSION 3.10)
project(ShowStream LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# # Enable static linking for standard libraries
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc")
# # Additional linker flags to prefer static linking
# set(CMAKE_EXE_LINKER_FLAGS "-static")

# message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")

include_directories(${CMAKE_SOURCE_DIR}/src/include)

# Use GLOB_RECURSE to find all .cpp files, including module files src/.../*.cpp
file(GLOB_RECURSE SOURCES "src/pkg/*.cpp")

set(WEBSITE "src/cmd/show-stream.cpp")
set(HASH_PASSWORD "src/cmd/hash-password.cpp")
set(RECORD_AUDIO "src/cmd/record-audio.cpp")

# add cmd entrypoints
add_executable(ShowStream ${SOURCES} ${WEBSITE})
add_executable(HashPassword ${SOURCES} ${HASH_PASSWORD})
add_executable(RecordAudio ${RECORD_AUDIO})

# Include directories for headers
target_include_directories(ShowStream PRIVATE include)
target_include_directories(RecordAudio PRIVATE include)

# Find ALSA library
find_package(ALSA REQUIRED)

# Link ALSA library to RecordAudio target
target_link_libraries(RecordAudio PRIVATE ${ALSA_LIBRARIES})

# Find the OpenSSL package
find_package(OpenSSL REQUIRED)

# Link OpenSSL to your executable
target_link_libraries(ShowStream PRIVATE OpenSSL::SSL OpenSSL::Crypto)
target_link_libraries(HashPassword PRIVATE OpenSSL::SSL OpenSSL::Crypto)
