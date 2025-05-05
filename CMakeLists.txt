cmake_minimum_required(VERSION 4.0.0)
project(
    tape-sort 
    VERSION 1.0 
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

enable_testing()

add_subdirectory(lib/tape)

add_executable(${PROJECT_NAME} src/main.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC lib/tape/include)
target_link_libraries(${PROJECT_NAME} PUBLIC tape)

add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                        ${PROJECT_SOURCE_DIR}/tape_config.txt $<TARGET_FILE_DIR:${PROJECT_NAME}>) 