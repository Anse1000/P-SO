cmake_minimum_required(VERSION 3.16)
project(P_SO C)

set(CMAKE_C_STANDARD 99)

add_executable(P_SO main.c list.c list.h commands.c commands.h aux-fun.c aux-fun.h)
