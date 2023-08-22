
FetchContent_Declare(
        cxxopts
        GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
        GIT_TAG v3.1.1
)

FetchContent_Declare(
        cpr
        GIT_REPOSITORY https://github.com/libcpr/cpr.git
        GIT_TAG 1.10.1
)

FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 10.0.0
)

FetchContent_Declare(
        indicators
        GIT_REPOSITORY https://github.com/p-ranav/indicators.git
        GIT_TAG v2.3
)


FetchContent_MakeAvailable(cxxopts)
FetchContent_MakeAvailable(fmt)
FetchContent_MakeAvailable(cpr)
FetchContent_MakeAvailable(indicators)

add_executable(arango_bench
        src/main.cpp
        include/Input.h
        include/Tools.h
        include/ArangoBench.h
        src/ArangoBench.cpp
        include/docker/Controller.h
        src/docker/Controller.cpp
        src/docker/Types.cpp
        include/docker/Types.h
        src/IController.cpp
        include/IController.h
        src/JsonGenerator.cpp
        include/JsonGenerator.h
        include/static_countries.h
        include/static_names.h
        include/ProgressLine.h
)
target_link_libraries(arango_bench PUBLIC arango-cpp cxxopts cpr::cpr fmt::fmt indicators::indicators)
target_include_directories(arango_bench PRIVATE ./include ./include/docker)
