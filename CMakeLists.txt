cmake_minimum_required(VERSION 3.20)

project(nano-itch)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(Nasdaq-itch-parser ${SOURCES})