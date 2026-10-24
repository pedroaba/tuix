cmake_minimum_required(VERSION 3.20)

project(tuix LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Gera as informações para o Clangd do Cursor
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Localiza a SDL3
find_package(SDL3 REQUIRED CONFIG)

add_executable(tuix
    src/main.cpp
)

# Vincula a biblioteca SDL3
target_link_libraries(tuix PRIVATE
    SDL3::SDL3
)

# Disponibiliza os headers da engine
target_include_directories(tuix PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)