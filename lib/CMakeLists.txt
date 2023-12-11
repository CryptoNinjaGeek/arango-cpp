
FetchContent_Declare(
        cpr
        GIT_REPOSITORY https://github.com/libcpr/cpr.git
        GIT_TAG 1.10.1
)

FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.2
)

FetchContent_Declare(
        jsoncons
        GIT_REPOSITORY https://github.com/danielaparker/jsoncons.git
        GIT_TAG v0.170.0
)


FetchContent_MakeAvailable(jsoncons)
FetchContent_MakeAvailable(json)
FetchContent_MakeAvailable(cpr)


add_library(arango-cpp SHARED
        src/Connection.cpp
        src/Database.cpp
        src/Collection.cpp
        src/Graph.cpp
        src/Request.cpp
        src/Response.cpp
        src/Index.cpp
        src/Cursor.cpp
        include/arango-cpp/input/DocumentInput.h
        include/arango-cpp/input/DocumentInput.h
        include/arango-cpp/input/IndexInput.h
        include/arango-cpp/Cursor.h
        src/Backup.cpp
        include/arango-cpp/Backup.h
        include/arango-cpp/input/BackupInput.h
        include/arango-cpp/Administration.h
        src/Administration.cpp
)

add_library(arangocpp::arangocpp ALIAS arango-cpp)

target_link_libraries(arango-cpp PRIVATE
        cpr::cpr
)

target_link_libraries(arango-cpp PUBLIC
        jsoncons
)

target_include_directories(arango-cpp PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/
)
