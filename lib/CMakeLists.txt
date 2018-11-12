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

if(CMAKE_HOST_WIN32)
    set(libname "libconfig")
else()
    set(libname "config")
endif()

add_library(${libname} ${libsrc} ${libinc})
add_library(${libname}++ ${libsrc_cpp} ${libinc_cpp})

set_target_properties(${libname}
    PROPERTIES LINKER_LANGUAGE C
        PUBLIC_HEADER "${libinc}")
set_target_properties(${libname}++
    PROPERTIES LINKER_LANGUAGE CXX
        PUBLIC_HEADER "${libinc_cpp}")

check_symbol_exists(uselocale "locale.h" HAVE_USELOCALE)
check_symbol_exists(newlocale "locale.h" HAVE_NEWLOCALE)
check_symbol_exists(freelocale "locale.h" HAVE_FREELOCALE)

if(HAVE_USELOCALE)
target_compile_definitions(${libname}
    PRIVATE "HAVE_USELOCALE")
endif()

if(HAVE_NEWLOCALE)
target_compile_definitions(${libname}
    PRIVATE "HAVE_NEWLOCALE")
endif()

if(HAVE_FREELOCALE)
target_compile_definitions(${libname}
    PRIVATE "HAVE_FREELOCALE")
endif()

target_compile_definitions(${libname}
    PRIVATE
        _CRT_SECURE_NO_DEPRECATE
        YY_NO_UNISTD_H
        YY_USE_CONST )

target_compile_definitions(${libname}++
    PRIVATE
        _CRT_SECURE_NO_DEPRECATE
        YY_NO_UNISTD_H
        YY_USE_CONST )

if(CMAKE_HOST_WIN32)
    target_link_libraries(${libname} Shlwapi.lib)
    target_link_libraries(${libname}++ Shlwapi.lib)
endif()

install(TARGETS ${libname}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(TARGETS ${libname}++
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
