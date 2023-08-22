

add_executable(connection_demo connection_demo.cpp)
target_link_libraries(connection_demo PUBLIC arango-cpp)

add_executable(insert_demo insert_demo.cpp)
target_link_libraries(insert_demo PUBLIC arango-cpp)

add_executable(aql_demo aql_demo.cpp)
target_link_libraries(aql_demo PUBLIC arango-cpp)
