function(set_version_info_from_makefile MAKEFILE_PATH TARGET_NAME)
    file(READ "${MAKEFILE_PATH}" MAKEFILE_CONTENTS)

    string(REGEX MATCH "VERINFO *= *-version-info +([0-9]+):([0-9]+):([0-9]+)" _ "${MAKEFILE_CONTENTS}")

    if(NOT CMAKE_MATCH_1)
        message(FATAL_ERROR "No valid VERINFO line found in ${MAKEFILE_PATH}")
    endif()

    set(CURRENT   ${CMAKE_MATCH_1})
    set(REVISION  ${CMAKE_MATCH_2})
    set(AGE       ${CMAKE_MATCH_3})

    math(EXPR SOVERSION "${CURRENT} - ${AGE}")
    set(VERSION "${SOVERSION}.${AGE}.${REVISION}")

    set_target_properties(${TARGET_NAME} PROPERTIES
        VERSION ${VERSION}
        SOVERSION ${SOVERSION}
    )

    message(STATUS "Set version info for ${TARGET_NAME}: VERSION=${VERSION}, SOVERSION=${SOVERSION}")
endfunction()

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

if(BUILD_CXX)
  add_library(${libname}++ ${libsrc_cpp} ${libinc_cpp})
endif()

target_include_directories(${libname} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

if(BUILD_CXX)
  target_include_directories(${libname}++ PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
      $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
endif()

set_target_properties(${libname}
    PROPERTIES LINKER_LANGUAGE C
        DEFINE_SYMBOL LIBCONFIG_EXPORTS
        PUBLIC_HEADER "${libinc}")
if(BUILD_CXX)
  set_target_properties(${libname}++
      PROPERTIES LINKER_LANGUAGE CXX
          DEFINE_SYMBOL LIBCONFIGXX_EXPORTS
          PUBLIC_HEADER "${libinc_cpp}")
endif()

set_version_info_from_makefile("Makefile.am" ${libname})
if(BUILD_CXX)
  set_version_info_from_makefile("Makefile.am" ${libname}++)
endif()

if(BUILD_SHARED_LIBS)
    if(BUILD_CXX)
      target_compile_definitions(${libname}++ PRIVATE LIBCONFIG_STATIC)
    endif()
else()
    target_compile_definitions(${libname} PUBLIC LIBCONFIG_STATIC)
    if(BUILD_CXX)
      target_compile_definitions(${libname}++ PUBLIC LIBCONFIG_STATIC LIBCONFIGXX_STATIC)
    endif()
endif()

if(APPLE)
    check_symbol_exists(uselocale "xlocale.h" HAVE_USELOCALE)
    check_symbol_exists(newlocale "xlocale.h" HAVE_NEWLOCALE)
    check_symbol_exists(freelocale "xlocale.h" HAVE_FREELOCALE)
else()
    check_symbol_exists(uselocale "locale.h" HAVE_USELOCALE)
    check_symbol_exists(newlocale "locale.h" HAVE_NEWLOCALE)
    check_symbol_exists(freelocale "locale.h" HAVE_FREELOCALE)
endif()

if(HAVE_USELOCALE)
    target_compile_definitions(${libname}
        PRIVATE "HAVE_USELOCALE")
    if(BUILD_CXX)
      target_compile_definitions(${libname}++
          PRIVATE "HAVE_USELOCALE")
    endif()
endif()

if(HAVE_NEWLOCALE)
    target_compile_definitions(${libname}
        PRIVATE "HAVE_NEWLOCALE")
    if(BUILD_CXX)
      target_compile_definitions(${libname}++
          PRIVATE "HAVE_NEWLOCALE")
    endif()
endif()

if(HAVE_FREELOCALE)
    target_compile_definitions(${libname}
        PRIVATE "HAVE_FREELOCALE")
    if(BUILD_CXX)
      target_compile_definitions(${libname}++
          PRIVATE "HAVE_FREELOCALE")
    endif()
endif()

if(MSVC)
    target_compile_definitions(${libname}
        PRIVATE
            _CRT_SECURE_NO_DEPRECATE
            YY_NO_UNISTD_H
            YY_USE_CONST )
    if(BUILD_CXX)
      target_compile_definitions(${libname}++
          PRIVATE
              _CRT_SECURE_NO_DEPRECATE
              YY_NO_UNISTD_H
              YY_USE_CONST )
    endif()
endif()

if(WIN32)
    target_link_libraries(${libname} shlwapi)
    if(BUILD_CXX)
      target_link_libraries(${libname}++ shlwapi)
    endif()
endif()

target_include_directories(${libname}
  PUBLIC "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
  )
if(BUILD_CXX)
  target_include_directories(${libname}++
    PUBLIC "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    )
endif()

if(BUILD_CXX)
  set(_install_targets ${libname} ${libname}++)
else()
  set(_install_targets ${libname})
endif()

install(TARGETS ${_install_targets}
    EXPORT libconfigTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)


include(CMakePackageConfigHelpers)
write_basic_package_version_file("libconfigConfigVersion.cmake"
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMajorVersion
    )

install(EXPORT libconfigTargets
    FILE "libconfigConfig.cmake"
    NAMESPACE libconfig::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libconfig
    )

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/libconfigConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/libconfig"
    )

if (UNIX)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/libconfig.pc.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/libconfig.pc @ONLY
        )
    if(BUILD_CXX)
      configure_file(
          ${CMAKE_CURRENT_SOURCE_DIR}/libconfig++.pc.cmake.in
          ${CMAKE_CURRENT_BINARY_DIR}/libconfig++.pc @ONLY
          )
    endif()
    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/libconfig.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
        )
    if(BUILD_CXX)
      install(FILES
          ${CMAKE_CURRENT_BINARY_DIR}/libconfig++.pc
          DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
          )
    endif()
endif (UNIX)
