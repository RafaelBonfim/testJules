# CMake minimum version
cmake_minimum_required(VERSION 3.10)

# Project name
project(injector)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable
add_executable(injector injector.cpp)

# Link against necessary libraries (Kernel32 is usually linked by default on MSVC)
# target_link_libraries(injector kernel32)

# Optional: Set output directory for the executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
