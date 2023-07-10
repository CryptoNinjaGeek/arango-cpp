
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

FetchContent_MakeAvailable(cxxopts)

add_executable(arango_bench
        src/main.cpp
        include/ProgressBar.h
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
)
target_link_libraries(arango_bench PUBLIC zutano cxxopts cpr::cpr)
target_include_directories(arango_bench PRIVATE ./include ./include/docker)
