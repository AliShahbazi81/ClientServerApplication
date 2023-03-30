cmake_minimum_required(VERSION 3.24)
project(ASP_Project C)

set(CMAKE_C_STANDARD 11)

add_executable(ASP_Project Server.c common.h common.c client.c mirror.c)
