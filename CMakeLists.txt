cmake_minimum_required(VERSION 3.10)
project(js_compiler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(js_compiler
    src/main.cpp
    src/lexer.cpp
    src/parser.cpp
)
