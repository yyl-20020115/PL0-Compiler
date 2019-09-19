cmake_minimum_required(VERSION 3.0)

project(pl0_compiler++)

add_executable(pl0_compiler++ main.cpp PL0-Compiler++.cpp)

install(TARGETS pl0_compiler++ RUNTIME DESTINATION bin)

