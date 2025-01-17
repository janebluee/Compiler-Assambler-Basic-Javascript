cmake_minimum_required(VERSION 3.10)
project(js_compiler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if(MSVC)
    add_compile_options(/O2 /GL /Gy /Oi /Ot)
    add_link_options(/LTCG)
else()
    add_compile_options(-O3 -march=native -flto)
    add_link_options(-flto)
endif()

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(js_compiler
    src/main.cpp
    src/lexer.cpp
    src/parser.cpp
    src/optimizer.cpp
    src/ast.cpp
)
