add_executable(c_example1 example1.c )
add_executable(c_example2 example2.c )
add_executable(c_example3 example3.c )

target_include_directories(c_example1 PRIVATE ${CMAKE_SOURCE_DIR}/lib)
target_include_directories(c_example2 PRIVATE ${CMAKE_SOURCE_DIR}/lib)
target_include_directories(c_example3 PRIVATE ${CMAKE_SOURCE_DIR}/lib)

target_link_libraries(c_example1 libconfig )
target_link_libraries(c_example2 libconfig )
target_link_libraries(c_example3 libconfig )