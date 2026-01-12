cmake_minimum_required(VERSION 3.5)

project(Pacman LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Pacman main.cc pacman.cc pacman.hh   )

install(TARGETS Pacman
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
