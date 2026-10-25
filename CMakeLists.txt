
cmake_minimum_required(VERSION 3.20)

project(tuix LANGUAGES CXX)

# ==========================================
# C++ Configuration
# ==========================================

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Generate compile_commands.json for Clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# ==========================================
# Dependencies
# ==========================================

# SDL3
find_package(SDL3 REQUIRED CONFIG)

# Skia
set(SKIA_DIR "" CACHE PATH "Path to Skia")

if(NOT EXISTS "${SKIA_DIR}/out/Learning/libskia.a")
    message(FATAL_ERROR "Skia library not found")
endif()

add_library(skia STATIC IMPORTED)

set_target_properties(skia PROPERTIES
    IMPORTED_LOCATION
        "${SKIA_DIR}/out/Learning/libskia.a"

    INTERFACE_INCLUDE_DIRECTORIES
        "${SKIA_DIR}"
)


# ==========================================
# Tuix Executable
# ==========================================

add_executable(tuix
    src/main.cpp
    src/rendering/skia_renderer.cpp
    src/rendering/render_context.cpp
)

# Engine headers
target_include_directories(tuix PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)


# ==========================================
# Link Libraries
# ==========================================

target_link_libraries(tuix PRIVATE
    SDL3::SDL3
    skia
)

# macOS Frameworks
if(APPLE)
    target_link_libraries(tuix PRIVATE
        "-framework AppKit"
        "-framework ApplicationServices"
        "-framework CoreFoundation"
        "-framework CoreGraphics"
        "-framework CoreText"
    )
endif()
