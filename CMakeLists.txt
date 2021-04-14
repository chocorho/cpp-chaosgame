cmake_minimum_required(VERSION 3.17)
project(project)

set(CMAKE_CXX_STANDARD 11)

link_libraries(Gdi32)

add_executable(project implementation.cpp main.cpp layout.rc)
