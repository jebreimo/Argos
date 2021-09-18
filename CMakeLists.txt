# ===========================================================================
# Copyright © 2020 Jan Erik Breimo. All rights reserved.
# Created by Jan Erik Breimo on 2020-01-07.
#
# This file is distributed under the BSD License.
# License text is included with the source distribution.
# ===========================================================================
cmake_minimum_required(VERSION 3.16)
project(Argos VERSION 1.1.2)

set(CMAKE_CXX_STANDARD 17)

# Set ARGOS_MASTER_PROJECT to ON if Argos is the root level project.
if(NOT DEFINED ARGOS_MASTER_PROJECT)
    if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        set(ARGOS_MASTER_PROJECT ON)
    else()
        set(ARGOS_MASTER_PROJECT OFF)
    endif()
endif()

# Test option
option(ARGOS_BUILD_TEST "Build tests" ${ARGOS_MASTER_PROJECT})

# Install option
option(ARGOS_INSTALL "Generate the install target" ${ARGOS_MASTER_PROJECT})

include(GNUInstallDirs)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

function(TargetEnableAllWarnings target)
    target_compile_options(${target}
        PRIVATE
            $<$<CXX_COMPILER_ID:Clang>:-Wall>
            $<$<CXX_COMPILER_ID:GNU>:-Wall>
            $<$<CXX_COMPILER_ID:AppleClang>:-Wall>
            $<$<CXX_COMPILER_ID:MSVC>:/W4>)
endfunction()

configure_file(src/Argos/ArgosVersion.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/ArgosVersion.hpp @ONLY)

add_library(Argos
    include/Argos/Argos.hpp
    include/Argos/ArgosException.hpp
    include/Argos/Argument.hpp
    include/Argos/ArgumentIterator.hpp
    include/Argos/ArgumentParser.hpp
    include/Argos/ArgumentValue.hpp
    include/Argos/ArgumentValues.hpp
    include/Argos/ArgumentValueIterator.hpp
    include/Argos/ArgumentView.hpp
    include/Argos/Callbacks.hpp
    include/Argos/Enums.hpp
    include/Argos/IArgumentView.hpp
    include/Argos/Option.hpp
    include/Argos/OptionView.hpp
    include/Argos/ParsedArguments.hpp
    include/Argos/ParsedArgumentsBuilder.hpp
    src/Argos/ArgosThrow.hpp
    src/Argos/Argument.cpp
    src/Argos/ArgumentCounter.cpp
    src/Argos/ArgumentCounter.hpp
    src/Argos/ArgumentData.hpp
    src/Argos/ArgumentIterator.cpp
    src/Argos/ArgumentIteratorImpl.cpp
    src/Argos/ArgumentIteratorImpl.hpp
    src/Argos/ArgumentParser.cpp
    src/Argos/ArgumentValue.cpp
    src/Argos/ArgumentValues.cpp
    src/Argos/ArgumentValueIterator.cpp
    src/Argos/ArgumentView.cpp
    src/Argos/ConsoleWidth.cpp
    src/Argos/ConsoleWidth.hpp
    src/Argos/HelpText.cpp
    src/Argos/HelpText.hpp
    src/Argos/IOptionIterator.hpp
    src/Argos/Option.cpp
    src/Argos/OptionData.hpp
    src/Argos/OptionIterator.cpp
    src/Argos/OptionIterator.hpp
    src/Argos/OptionView.cpp
    src/Argos/ParseValue.cpp
    src/Argos/ParseValue.hpp
    src/Argos/ParsedArguments.cpp
    src/Argos/ParsedArgumentsImpl.cpp
    src/Argos/ParsedArgumentsImpl.hpp
    src/Argos/ParsedArgumentsBuilder.cpp
    src/Argos/ParserData.hpp
    src/Argos/StandardOptionIterator.cpp
    src/Argos/StandardOptionIterator.hpp
    src/Argos/StringUtilities.cpp
    src/Argos/StringUtilities.hpp
    src/Argos/TextFormatter.cpp
    src/Argos/TextFormatter.hpp
    src/Argos/TextWriter.cpp
    src/Argos/TextWriter.hpp
    src/Argos/WordSplitter.cpp
    src/Argos/WordSplitter.hpp
    )

target_include_directories(Argos
    PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    )

TargetEnableAllWarnings(Argos)

add_library(Argos::Argos ALIAS Argos)

add_subdirectory(docs/doxygen EXCLUDE_FROM_ALL)

if(ARGOS_BUILD_TEST)
    include(CodeCoverage)
    TargetEnableCodeCoverage(Argos "${ARGOS_CODE_COVERAGE}")

    enable_testing(ARGOS_ENABLE_CODE_COVERAGE)

    add_subdirectory(tests/ArgosTest)
endif()

if(UPDATE_SINGLE_SRC)
    find_package(Python3 COMPONENTS Interpreter REQUIRED)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/tools/make_argos_cpp.sh.in
        make_argos_cpp.sh)
    add_custom_target(UpdateSingleSrc ALL
        BYPRODUCTS
            single_src/Argos/Argos.hpp
            single_src/Argos/Argos.cpp
        COMMAND
            sh "${CMAKE_CURRENT_BINARY_DIR}/make_argos_cpp.sh"
    )
    add_dependencies(UpdateSingleSrc Argos)
endif()

export(TARGETS Argos
    NAMESPACE Argos::
    FILE ArgosConfig.cmake)

if(ARGOS_INSTALL)
    install(TARGETS Argos
        EXPORT ArgosConfig
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        )

    install(EXPORT ArgosConfig
        FILE
            ArgosConfig.cmake
        NAMESPACE
            Argos::
        DESTINATION
            ${CMAKE_INSTALL_LIBDIR}/cmake/Argos
        )

    install(
        DIRECTORY
            include/Argos
        DESTINATION
            ${CMAKE_INSTALL_INCLUDEDIR}
        )

    install(
        FILES
            ${CMAKE_CURRENT_BINARY_DIR}/ArgosVersion.hpp
        DESTINATION
            ${CMAKE_INSTALL_INCLUDEDIR}/Argos
    )

    include(CMakePackageConfigHelpers)

    write_basic_package_version_file(ArgosConfigVersion.cmake
        COMPATIBILITY SameMajorVersion
        )

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ArgosConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Argos
        )
endif()
