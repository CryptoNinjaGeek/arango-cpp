cmake_minimum_required(VERSION 3.24)
project(arango-cpp CXX)

option(ZUTANO_TESTS "Build Tests" ON)
option(ZUTANO_EXAMPLES "Build EXAMPLES" ON)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

add_subdirectory(lib)
add_subdirectory(tools)

if (ZUTANO_TESTS)
    add_subdirectory(tests)
endif ()

if (ZUTANO_EXAMPLES)
    add_subdirectory(examples)
endif ()
