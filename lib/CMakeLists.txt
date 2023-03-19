

add_library(zutano SHARED
        src/library.cpp
        )

target_include_directories(zutano PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/
        )
