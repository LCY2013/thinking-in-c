cmake_minimum_required(VERSION 3.21)
project(thinking_in_c)

set(CMAKE_CXX_STANDARD 23)

add_executable(thinking_in_c untilc_17/c_grammar.c untilc_17/imperative_programming.c untilc_17/const_grammar.c untilc_17/cover_grammar.c)
