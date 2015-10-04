cmake_minimum_required(VERSION 3.3.2)
project(pong CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/")

add_subdirectory(cli)
add_subdirectory(lib)

