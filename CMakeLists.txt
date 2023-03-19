cmake_minimum_required(VERSION 3.25)
project(zutano CXX)

option(ZUTANO_TESTS "Build Tests" ON)
option(ZUTANO_EXAMPLES "Build EXAMPLES" ON)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

add_subdirectory(lib)

if (ZUTANO_TESTS)
    add_subdirectory(tests)
endif ()

add_subdirectory(examples)
if (ZUTANO_EXAMPLES)
    add_subdirectory(examples)
endif ()
