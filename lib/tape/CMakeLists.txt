add_library(tape src/file_tape.cpp src/itape.cpp)
target_include_directories(tape PUBLIC include)

set(TAPE_SOURCE_DIR ${PROJECT_SOURCE_DIR}/lib/tape)

add_subdirectory(test)