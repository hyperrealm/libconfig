add_library(libtinytest STATIC
	tinytest.h
	tinytest.c)

if(CMAKE_HOST_WIN32)
	if(MSVC)	
		set_target_properties(libtinytest
			PROPERTIES
				COMPILE_FLAGS "/wd4996"
			)
	endif()
endif()