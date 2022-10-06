#---------------------------------------------------------------------------//
#  MIT License
#
#  Copyright (c) 2020 Mikhail Komarov <nemo@nil.foundation>
#
#  Permission is hereby granted, free of charge, to any person obtaining a copy
#  of this software and associated documentation files (the "Software"), to deal
#  in the Software without restriction, including without limitation the rights
#  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#  copies of the Software, and to permit persons to whom the Software is
#  furnished to do so, subject to the following conditions:
#
#  The above copyright notice and this permission notice shall be included in all
#  copies or substantial portions of the Software.
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
#  SOFTWARE.
#---------------------------------------------------------------------------//


include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../include"
        "${CMAKE_CURRENT_BINARY_DIR}/include"

        ${Boost_INCLUDE_DIRS})

macro(define_placeholder_proof_gen_example example)

    set(target_name ${CMAKE_WORKSPACE_NAME}_${CURRENT_PROJECT_NAME}_${example}_example)
    string(REPLACE "/" "_" target_name ${target_name})
    string(REPLACE "-" "_" target_name ${target_name})

    add_executable(${target_name} ${example}.cpp)
    target_link_libraries(${target_name} PRIVATE
            crypto3::algebra
            crypto3::hash
            crypto3::blueprint
            crypto3::zk
            marshalling::core
            marshalling::crypto3_multiprecision
            marshalling::crypto3_algebra
            marshalling::crypto3_zk

            ${Boost_LIBRARIES})
    set_target_properties(${target_name} PROPERTIES CXX_STANDARD 17)
endmacro()

set(EXAMPLES_NAMES
    "circuit_serialization/unified_addition"
    "public_params_serialization/unified_addition"
    "proof_generate")

foreach(EXAMPLE_NAME ${EXAMPLES_NAMES})
    define_placeholder_proof_gen_example(${EXAMPLE_NAME})
endforeach()
