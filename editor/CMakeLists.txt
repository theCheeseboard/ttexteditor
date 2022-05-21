cmake_minimum_required(VERSION 3.4.0)

project(lib VERSION 1.0.0 LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(libcontemporary REQUIRED)

set(SOURCES
    texteditor.cpp
    textcaret.cpp
    texteditorcolorscheme.cpp
    commands/carettextcommand.cpp
    commands/careterasecommand.cpp
    commands/texteditorcommand.cpp
    texteditorlineproperties.cpp
    rendering/texteditorrenderstep.cpp
    rendering/compilerissuerenderstep.cpp
)

set(HEADERS
    texteditor.h
    texteditorcolorscheme.h
    rendering/texteditorrenderstep.h
)

set(PRIVATE_HEADERS
    texteditor_p.h
    textcaret.h
    commands/carettextcommand.h
    commands/careterasecommand.h
    commands/texteditorcommand.h
    rendering/compilerissuerenderstep.h
)

add_library(ttexteditor SHARED ${SOURCES} ${HEADERS} ${PRIVATE_HEADERS})
cntp_init(ttexteditor 17)
set_target_properties(ttexteditor PROPERTIES
        OUTPUT_NAME ttexteditor
        FRAMEWORK TRUE
        MACOSX_FRAMEWORK_IDENTIFIER com.vicr123.theslate
        VERSION 1.0.0
        PUBLIC_HEADER "${HEADERS}")

target_link_libraries(ttexteditor Qt6::Widgets libcontemporary)
target_compile_definitions(ttexteditor PRIVATE TTEXTEDITOR_LIBRARY)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(CMAKECONFIG_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/ttexteditor.framework/Resources/CMake)
    set(HEADER_INSTALL_DIR ${CMAKE_INSTALL_PREFIX})
    set(INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/ttexteditor.framework/Headers)
    set(LIBRARY_INSTALL_DIR ../)
ELSE()
    set(CMAKECONFIG_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/ttexteditor)
    set(HEADER_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR}/ttexteditor)
    set(INCLUDE_INSTALL_DIR ${HEADER_INSTALL_DIR})
    set(LIBRARY_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR})
ENDIF()

configure_package_config_file(ttexteditorConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/ttexteditorConfig.cmake
        INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
        PATH_VARS HEADER_INSTALL_DIR LIBRARY_INSTALL_DIR)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ttexteditorConfig.cmake
        DESTINATION ${CMAKECONFIG_INSTALL_DIR})
install(EXPORT ttexteditorTargets
        FILE ttexteditorTargets.cmake
        DESTINATION ${CMAKECONFIG_INSTALL_DIR})

install(TARGETS ttexteditor
        EXPORT ttexteditorTargets
        PUBLIC_HEADER DESTINATION ${HEADER_INSTALL_DIR}
        INCLUDES DESTINATION ${INCLUDE_INSTALL_DIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR})
