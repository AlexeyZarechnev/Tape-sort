add_library(tape src/file_tape.cpp src/itape.cpp)
target_include_directories(tape PUBLIC include)

add_subdirectory(test)