add_executable(libconfig_tests
    tests.c
)

target_link_libraries(libconfig_tests
    libconfig
    libtinytest
)

add_test(
    NAME libconfig_tests
    COMMAND libconfig_tests
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests
)
