//---------------------------------------------------------------------------//
// Copyright (c) 2018-2021 Mikhail Komarov <nemo@nil.foundation>
// Copyright (c) 2022 Aleksei Moskvin <alalmoskvin@nil.foundation>
// Copyright (c) 2022-2023 Ilia Shirobokov <i.shirobokov@nil.foundation>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//---------------------------------------------------------------------------//

#ifndef PROOF_GENERATOR_CIRCUITS_MINA_STATE_DESERIALIZATION_FP_ELEMENT_HPP
#define PROOF_GENERATOR_CIRCUITS_MINA_STATE_DESERIALIZATION_FP_ELEMENT_HPP

namespace nil {
    namespace crypto3 {
        namespace algebra {
            namespace fields {
                namespace detail {
                    template<typename T>
                    nil::crypto3::algebra::fields::detail::element_fp<T>
                        tag_invoke(boost::json::value_to_tag<nil::crypto3::algebra::fields::detail::element_fp<T>>,
                                   const boost::json::value &jv) {

                        multiprecision::cpp_int x(boost::json::value_to<std::string>(jv));
                        return nil::crypto3::algebra::fields::detail::element_fp<T>(x);
                    }
                }    // namespace detail
            }        // namespace fields
        }            // namespace algebra
    }                // namespace crypto3
}    // namespace nil

#endif    // PROOF_GENERATOR_CIRCUITS_MINA_STATE_DESERIALIZATION_FP_ELEMENT_HPP