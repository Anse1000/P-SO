cmake_minimum_required(VERSION 3.16)
project(p0_SO C)

set(CMAKE_C_STANDARD 99)

add_executable(p0_SO main.c list.c list.h commands.c commands.h aux-fun.c aux-fun.h)
