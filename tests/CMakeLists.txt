cmake_minimum_required(VERSION 3.10)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG main
)

FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 10.0.0
)

FetchContent_MakeAvailable(googletest)

add_executable(arango_test
        collection_test.cpp
        database_test.cpp
        backup_test.cpp
)

target_link_libraries(arango_test fmt::fmt gtest gtest_main gmock_main arango-cpp)

add_test(arango_test arango_test)