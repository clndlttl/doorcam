cmake_minimum_required(VERSION 3.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_COMPILER "/usr/bin/g++-4.9")

project(DOORCAM)

add_subdirectory(server)
add_subdirectory(client)
