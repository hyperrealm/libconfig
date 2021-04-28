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

if(MSVC)
    set(libname "libconfig")
else()
    set(libname "config")
endif()

add_library(${libname} ${libsrc} ${libinc})
add_library(${libname}++ ${libsrc_cpp} ${libinc_cpp})

set_target_properties(${libname}
    PROPERTIES LINKER_LANGUAGE C
        SOVERSION "${libconfig_VERSION_MAJOR}"
        VERSION "${libconfig_VERSION}"
        DEFINE_SYMBOL LIBCONFIG_EXPORTS
        PUBLIC_HEADER "${libinc}")
set_target_properties(${libname}++
    PROPERTIES LINKER_LANGUAGE CXX
        SOVERSION "${libconfig_VERSION_MAJOR}"
        DEFINE_SYMBOL LIBCONFIGXX_EXPORTS
        VERSION "${libconfig_VERSION}"
        PUBLIC_HEADER "${libinc_cpp}")

#check_symbol_exists(uselocale "locale.h" HAVE_USELOCALE)
#check_symbol_exists(newlocale "locale.h" HAVE_NEWLOCALE)
#check_symbol_exists(freelocale "locale.h" HAVE_FREELOCALE)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(${libname}++ PRIVATE LIBCONFIG_STATIC)
else()
    target_compile_definitions(${libname} PUBLIC LIBCONFIG_STATIC)
    target_compile_definitions(${libname}++ PUBLIC LIBCONFIGXX_STATIC)
endif()

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

if(MSVC)
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
endif()

if(WIN32)
    target_link_libraries(${libname} shlwapi)
    target_link_libraries(${libname}++ shlwapi)
endif()

target_include_directories(${libname}
  PUBLIC "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
  )

target_include_directories(${libname}++
  PUBLIC "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
  )

install(TARGETS ${libname}
    EXPORT libconfigTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(TARGETS ${libname}++
    EXPORT libconfig++Targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)


include(CMakePackageConfigHelpers)
foreach(target_name libconfig libconfig++)
  write_basic_package_version_file("${target_name}ConfigVersion.cmake"
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMajorVersion
    )

  install(EXPORT ${target_name}Targets
    FILE "${target_name}Config.cmake"
    NAMESPACE libconfig::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libconfig
    )

  install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${target_name}ConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/libconfig"
    )
endforeach()
