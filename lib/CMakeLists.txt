
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


add_library(zutano SHARED
        src/Connection.cpp
        src/Database.cpp
        src/Collection.cpp
        src/Request.cpp
        src/Response.cpp
        src/Index.cpp include/zutano/input/DocumentInput.h)

target_link_libraries(zutano PRIVATE
        cpr::cpr
        )

target_link_libraries(zutano PUBLIC
        jsoncons
        )

target_include_directories(zutano PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/
        )
