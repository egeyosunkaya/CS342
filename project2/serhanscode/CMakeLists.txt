cmake_minimum_required(VERSION 3.6)
project(Project2)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES app1.c tlib.c)
add_executable(Project2 ${SOURCE_FILES})