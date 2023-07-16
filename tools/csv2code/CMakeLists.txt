
FetchContent_Declare(
        cxxopts
        GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
        GIT_TAG v3.1.1
)


FetchContent_MakeAvailable(cxxopts)

add_executable(csv2code src/main.cpp)
target_link_libraries(csv2code PUBLIC cxxopts)
