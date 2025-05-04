include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(
    tape_tests
    tape_tests.cpp
)

target_include_directories(tape_tests PUBLIC ../include)

target_link_libraries(
    tape_tests
    tape
    GTest::gtest_main
)

add_custom_command(TARGET tape_tests PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${TAPE_SOURCE_DIR}/test/conf/ $<TARGET_FILE_DIR:tape_tests>/conf)

add_custom_command(TARGET tape_tests PRE_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                        ${TAPE_SOURCE_DIR}/test/data/ $<TARGET_FILE_DIR:tape_tests>/data)                      

include(GoogleTest)

gtest_discover_tests(tape_tests)