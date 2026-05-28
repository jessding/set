cmake_minimum_required(VERSION 4.1)

project(set_game)

set(CMAKE_CXX_STANDARD 26)

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_image REQUIRED)

add_executable(set_game
    src/main.cpp
    src/core/rules.cpp
    src/core/board.cpp
    src/core/game.cpp
    src/ui/sdl_renderer.cpp
)

target_include_directories(set_game PRIVATE include)

target_link_libraries(set_game PRIVATE
    SDL2::SDL2
    SDL2_ttf::SDL2_ttf
    SDL2_image::SDL2_image
)

file(COPY assets
     DESTINATION ${CMAKE_BINARY_DIR})