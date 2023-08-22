
FetchContent_Declare(
        cxxopts
        GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
        GIT_TAG v3.1.1
)


FetchContent_MakeAvailable(cxxopts)

add_executable(arango_import arango_import.cpp progressbar.h)
target_link_libraries(arango_import PUBLIC arango-cpp cxxopts)
