# CMake minimum version
cmake_minimum_required(VERSION 3.10)

# Project name
project(dx11hook)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the main DLL source files
add_library(dx11hook SHARED
    dllmain.cpp
    hooks.cpp
    # MinHook library files
    minhook/src/buffer.c
    minhook/src/hook.c
    minhook/src/trampoline.c
    minhook/src/hde/hde32.c
    minhook/src/hde/hde64.c
    # ImGui library files
    src/imgui/imgui.cpp
    src/imgui/imgui_demo.cpp
    src/imgui/imgui_draw.cpp
    src/imgui/imgui_tables.cpp
    src/imgui/imgui_widgets.cpp
    src/imgui/imgui_impl_win32.cpp
    src/imgui/imgui_impl_dx11.cpp
)

# Add include directories
target_include_directories(dx11hook PUBLIC
    ./
    minhook/include
    src/imgui
)

# Define preprocessor definitions
target_compile_definitions(dx11hook PRIVATE IMGUI_VERSION="1.90.8 WIP")
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    target_compile_definitions(dx11hook PRIVATE _M_X64)
else()
    target_compile_definitions(dx11hook PRIVATE _M_IX86)
endif()

# Link against necessary libraries
target_link_libraries(dx11hook
    d3d11
    dxgi
)

# Optional: Set output directory for the DLL
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add the injector subdirectory
add_subdirectory(injector)
