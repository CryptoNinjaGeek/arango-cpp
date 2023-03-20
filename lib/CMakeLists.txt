
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

FetchContent_MakeAvailable(json)
FetchContent_MakeAvailable(cpr)


add_library(zutano SHARED
        src/connection.cpp
        src/database.cpp
        src/collection.cpp)

target_link_libraries(zutano PRIVATE
        cpr::cpr
        )

target_link_libraries(zutano PUBLIC
        nlohmann_json::nlohmann_json
        )

target_include_directories(zutano PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/
        )
