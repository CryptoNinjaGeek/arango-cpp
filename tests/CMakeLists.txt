
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.3.2 # or a later release
)

FetchContent_MakeAvailable(Catch2)

add_executable(tests demo_test.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
include(CTest)
include(Catch)
catch_discover_tests(tests)