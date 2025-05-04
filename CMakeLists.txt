cmake_minimum_required(VERSION 4.0.0)
project(
    tape-sort 
    VERSION 1.0 
    LANGUAGES CXX
)

set(CXX_COMPILER_VERSION 17)

add_subdirectory(lib/tape)

add_executable(main src/main.cpp)
target_include_directories(main PUBLIC lib/tape/include)
target_link_libraries(main PUBLIC tape)