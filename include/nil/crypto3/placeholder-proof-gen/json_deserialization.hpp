//---------------------------------------------------------------------------//
// Copyright (c) 2022 Mikhail Komarov <nemo@nil.foundation>
// Copyright (c) 2022 Aleksei Moskvin <alalmoskvin@nil.foundation>
//
// MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//---------------------------------------------------------------------------//

#ifndef PLACEHOLDER_PROOF_GEN_JSON_DESERIALIZATION_HPP
#define PLACEHOLDER_PROOF_GEN_JSON_DESERIALIZATION_HPP

#include <boost/json/src.hpp>

namespace nil {
    namespace crypto3 {
        namespace placeholder_proof_gen {
            struct arithmetization_params_t {
                size_t witness_columns;
                size_t public_input_columns;
                size_t constant_columns;
                size_t selector_columns;

                arithmetization_params_t (const boost::json::value &jv) {
                    witness_columns = boost::json::value_to<std::size_t>(jv.at("witness_columns"));
                    public_input_columns = boost::json::value_to<std::size_t>(jv.at("public_input_columns"));
                    constant_columns = boost::json::value_to<std::size_t>(jv.at("constant_columns"));
                    selector_columns = boost::json::value_to<std::size_t>(jv.at("selector_columns"));
                };

                arithmetization_params_t (std::size_t witness_columns_in, std::size_t public_input_columns_in,
                                          std::size_t constant_columns_in, std::size_t selector_columns_in) :
                    witness_columns(witness_columns_in), public_input_columns(public_input_columns_in),
                    constant_columns(constant_columns_in), selector_columns(selector_columns_in) { };

                arithmetization_params_t () { };

                bool operator==(const arithmetization_params_t& rhs)
                {
                    return witness_columns == rhs.witness_columns && public_input_columns == rhs.public_input_columns &&
                            constant_columns == rhs.constant_columns && selector_columns == rhs.selector_columns;
                }
            };
        }    // namespace math
    }        // namespace crypto3
}    // namespace nil

#endif    // PLACEHOLDER_PROOF_GEN_JSON_DESERIALIZATION_HPP
