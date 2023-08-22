
FetchContent_Declare(
        cxxopts
        GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
        GIT_TAG v3.1.1
)


FetchContent_MakeAvailable(cxxopts)

add_executable(arango_import src/main.cpp include/ProgressBar.h include/Input.h include/Tools.h include/ArangoImport.h src/ArangoImport.cpp src/ImportCSV.cpp)
target_link_libraries(arango_import PUBLIC arango-cpp cxxopts)
target_include_directories(arango_import PRIVATE ./include)
