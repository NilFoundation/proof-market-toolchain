#---------------------------------------------------------------------------#
# Copyright (c) 2018-2020 Mikhail Komarov <nemo@nil.foundation>
#
# Distributed under the Boost Software License, Version 1.0
# See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt
#---------------------------------------------------------------------------#

include(CMTest)

if(NOT Boost_UNIT_TEST_FRAMEWORK_FOUND)
    cm_find_package(Boost REQUIRED COMPONENTS unit_test_framework)
endif()

cm_test_link_libraries(${CMAKE_WORKSPACE_NAME}_${CURRENT_PROJECT_NAME}

                       ${CMAKE_WORKSPACE_NAME}::algebra
                       ${CMAKE_WORKSPACE_NAME}::blueprint
                       ${CMAKE_WORKSPACE_NAME}::zk

                       ${Boost_LIBRARIES})

macro(define_test name)
    cm_test(NAME ${CMAKE_WORKSPACE_NAME}_${CURRENT_PROJECT_NAME}_${name}_test SOURCES ${name}.cpp)

    target_include_directories(${CMAKE_WORKSPACE_NAME}_${CURRENT_PROJECT_NAME}_${name}_test PRIVATE
                               "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                               "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>"

                               ${Boost_INCLUDE_DIRS})

    set_target_properties(${CMAKE_WORKSPACE_NAME}_${CURRENT_PROJECT_NAME}_${name}_test PROPERTIES CXX_STANDARD 17)

    get_target_property(target_type Boost::unit_test_framework TYPE)
    if(target_type STREQUAL "SHARED_LIB")
        target_compile_definitions(math_${name}_test PRIVATE BOOST_TEST_DYN_LINK)
    elseif(target_type STREQUAL "STATIC_LIB")

    endif()
endmacro()

set(TESTS_NAMES
    "proof_generate"
    "serialization")

foreach(TEST_NAME ${TESTS_NAMES})
    define_test(${TEST_NAME})
endforeach()
