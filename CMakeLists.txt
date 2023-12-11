cmake_minimum_required(VERSION 3.24)
project(arango-cpp CXX)

option(ARANGO_CPP_TESTS "Build Tests" ON)
option(ARANGO_CPP_EXAMPLES "Build EXAMPLES" ON)
option(ARANGO_ENTERPRISE "Build enterprise tests" OFF)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

add_subdirectory(lib)
add_subdirectory(tools)

if (ARANGO_CPP_TESTS)
    add_subdirectory(tests)
endif ()

if (ARANGO_CPP_EXAMPLES)
    add_subdirectory(examples)
endif ()
