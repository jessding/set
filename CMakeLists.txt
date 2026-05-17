cmake_minimum_required(VERSION 4.1)

project(set_game)

set(CMAKE_CXX_STANDARD 26)

add_executable(set_game
    main.cpp
    rules.cpp
    board.cpp
)