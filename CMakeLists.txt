cmake_minimum_required(VERSION 4.0)
project(calculator C)

set(CMAKE_C_STANDARD 17)

add_executable(calculator main.c
        ast.c
        ast.h)
