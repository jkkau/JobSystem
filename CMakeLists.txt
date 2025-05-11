cmake_minimum_required(VERSION 3.14)
project(job)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(-Wall -Werror)

# 单元测试
set(SOURCES
    demo.cc
)

add_executable(job ${SOURCES})
target_include_directories(job PRIVATE ${CMAKE_SOURCE_DIR})
