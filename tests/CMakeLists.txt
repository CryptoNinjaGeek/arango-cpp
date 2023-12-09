cmake_minimum_required(VERSION 3.10)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG main
)
FetchContent_MakeAvailable(googletest)

add_executable(collection_test demo_test.cpp)
target_link_libraries(collection_test gtest gtest_main arango-cpp)

add_test(collection_test collection_test)