#---------------------------------------------------------------------------#
# Copyright (c) 2022 Mikhail Komarov <nemo@nil.foundation>
# Copyright (c) 2022 Aleksei Moskvin <alalmoskvin@nil.foundation>
#
# Distributed under the Boost Software License, Version 1.0
# See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt
#---------------------------------------------------------------------------#

cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

cmake_policy(SET CMP0042 NEW)
cmake_policy(SET CMP0028 NEW)
cmake_policy(SET CMP0057 NEW)


find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake"
        "${CMAKE_CURRENT_LIST_DIR}/cmake/packages"
        "${CMAKE_CURRENT_LIST_DIR}/cmake/modules/share/modules/cmake")

include(CMConfig)
include(CMDeploy)
include(CMSetupVersion)

cm_workspace(placeholder-proof-gen)

macro(cm_find_package NAME)
    if(NOT "${NAME}" MATCHES "^${CMAKE_WORKSPACE_NAME}_.*$" AND NOT "${NAME}" STREQUAL CM)
        find_package(${ARGV})
    else()
        set(${ARGV0}_FOUND ON CACHE BOOL "")
    endif()
endmacro()

option(BUILD_TESTS "Build unit tests" FALSE)
option(BUILD_SHARED_LIBS "Build shared library" TRUE)
option(CIRCUIT_ASSEMBLY_OUTPUT "Emit circuit IR as assembly file" FALSE)

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/libs/zkllvm-circifier/llvm")
add_subdirectories("${CMAKE_CURRENT_LIST_DIR}/libs")
add_subdirectories("${CMAKE_CURRENT_LIST_DIR}/libs/actor")
add_subdirectories("${CMAKE_CURRENT_LIST_DIR}/bin")
add_subdirectories("${CMAKE_CURRENT_LIST_DIR}/libs/marshalling")
add_subdirectory(example)