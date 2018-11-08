cmake_minimum_required(VERSION 3.7.0)
project(libirods_filesystem LANGUAGES CXX C)

set(TARGET irods_filesystem)

add_library(${TARGET} ${CMAKE_SOURCE_DIR}/src/path.cpp)
