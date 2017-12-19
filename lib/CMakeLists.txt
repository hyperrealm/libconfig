set(libinc
    libconfig.h)

set(libsrc
    grammar.h
    parsectx.h
    scanctx.h
    scanner.h
    win32/stdint.h
    strbuf.h
    strvec.h
    util.h
    wincompat.h
    grammar.c
    libconfig.c
    scanctx.c
    scanner.c
    strbuf.c
    strvec.c
    util.c
	wincompat.c)
	
set(libinc_cpp
    libconfig.h++
    libconfig.hh)

set(libsrc_cpp
    ${libsrc}
    libconfigcpp.cc)

add_library(libconfig ${LIB_TYPE} ${libsrc} ${libinc})
add_library(libconfig++ ${LIB_TYPE} ${libsrc_cpp} ${libinc_cpp})

set_target_properties(libconfig 
    PROPERTIES LINKER_LANGUAGE C
        PUBLIC_HEADER "${libinc}")
set_target_properties(libconfig++
    PROPERTIES LINKER_LANGUAGE CXX
        PUBLIC_HEADER "${libinc_cpp}")

target_compile_definitions(libconfig
    PRIVATE
        _CRT_SECURE_NO_DEPRECATE
        _STDLIB_H
        YY_NO_UNISTD_H
        YY_USE_CONST )

target_compile_definitions(libconfig++ 
    PRIVATE
        _CRT_SECURE_NO_DEPRECATE
        _STDLIB_H
        YY_NO_UNISTD_H
        YY_USE_CONST )

if(CMAKE_HOST_WIN32)
    target_link_libraries(libconfig Shlwapi.lib)
    target_link_libraries(libconfig++ Shlwapi.lib)
endif()

install(TARGETS libconfig
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(TARGETS libconfig++
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)