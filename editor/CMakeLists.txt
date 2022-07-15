cmake_minimum_required(VERSION 3.4.0)

project(lib VERSION 1.0.0 LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGLWidgets)
find_package(libcontemporary REQUIRED)

set(SOURCES
    texteditor.cpp
    textcaret.cpp
    completetexteditor.cpp completetexteditor.ui
    texteditorcolorscheme.cpp
    commands/carettextcommand.cpp
    commands/careterasecommand.cpp
    commands/texteditorcommand.cpp
    commands/textreplacementcommand.cpp
    texteditorlineproperties.cpp
    rendering/texteditorrenderstep.cpp
    rendering/compilerissuerenderstep.cpp
    rendering/caretrenderstep.cpp
    rendering/linetextrenderstep.cpp
    rendering/leftguttertextrenderstep.cpp
    rendering/compilerissuebackgroundrenderstep.cpp
    rendering/activelinebackgroundrenderstep.cpp
    rendering/texteditorperlinerenderstep.cpp
    statusbar/statusbarbutton.cpp
    commands/carettabcommand.cpp
    merge/textmergetool.cpp
    merge/conflictresolutiontexteditorrenderstep.cpp

    merge/textmergetool.ui
)

set(HEADERS
    ttexteditor_global.h
    texteditor.h
    completetexteditor.h
    texteditorcolorscheme.h
    rendering/texteditorrenderstep.h
    rendering/texteditorperlinerenderstep.h
    merge/textmergetool.h
)

set(PRIVATE_HEADERS
    texteditor_p.h
    textcaret.h
    commands/carettextcommand.h
    commands/careterasecommand.h
    commands/texteditorcommand.h
    commands/carettabcommand.h
    commands/textreplacementcommand.h
    rendering/compilerissuerenderstep.h
    rendering/caretrenderstep.h
    rendering/leftguttertextrenderstep.h
    rendering/compilerissuebackgroundrenderstep.h
    rendering/activelinebackgroundrenderstep.h
    rendering/linetextrenderstep.h
    statusbar/statusbarbutton.h
    merge/conflictresolutiontexteditorrenderstep.h
    merge/differ.h
)

add_library(ttexteditor SHARED ${SOURCES} ${HEADERS} ${PRIVATE_HEADERS})
cntp_init(ttexteditor 20)
set_target_properties(ttexteditor PROPERTIES
        OUTPUT_NAME ttexteditor
        FRAMEWORK TRUE
        MACOSX_FRAMEWORK_IDENTIFIER com.vicr123.theslate
        VERSION 1.0.0)

target_link_libraries(ttexteditor Qt6::Widgets Qt6::OpenGLWidgets libcontemporary)
target_compile_definitions(ttexteditor PRIVATE TTEXTEDITOR_LIBRARY)
cntp_target_public_headers(ttexteditor DESTINATION_DIRECTORY ttexteditor
    HEADERS ${HEADERS})

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(CMAKECONFIG_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/ttexteditor.framework/Resources/CMake)
    set(HEADER_INSTALL_DIR ${CMAKE_INSTALL_PREFIX})
    set(LIBRARY_INSTALL_DIR ../)
ELSE()
    set(CMAKECONFIG_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/ttexteditor)
    set(HEADER_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR}/ttexteditor)
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
        INCLUDES DESTINATION ${ttexteditor_INCLUDE_DIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR})
