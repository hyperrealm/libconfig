add_executable(libconfig_tests
	tests.c )
	
target_link_libraries(libconfig_tests
	libconfig
	libtinytest )

target_include_directories(libconfig_tests
	PRIVATE ${CMAKE_SOURCE_DIR}/lib
		${CMAKE_SOURCE_DIR}/tinytest )

add_test(NAME libconfig_tests COMMAND libconfig_tests)