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

#ifndef PROOF_GENERATOR_CIRCUITS_MINA_STATE_DESERIALIZATION_POINT_HPP
#define PROOF_GENERATOR_CIRCUITS_MINA_STATE_DESERIALIZATION_POINT_HPP

#include <nil/crypto3/multiprecision/cpp_int.hpp>

#include <nil/proof-generator/circuits/mina-state/deserialization/fp_element.hpp>

namespace nil {
    namespace proof_generator {
        namespace mina_state {

            template<typename Iterator>
            nil::crypto3::multiprecision::cpp_int get_cppui256(Iterator it) {
                BOOST_ASSERT(it->second.template get_value<std::string>() != "");
                return nil::crypto3::multiprecision::cpp_int(it->second.template get_value<std::string>());
            }

            template<typename CurveType>
            void check_coord(typename CurveType::base_field_type::value_type &x,
                             typename CurveType::base_field_type::value_type &y) {
                if (x == 0 && y == 1) {    // circuit uses (0, 0) as point-at-infinity
                    y = 0;
                }

                typename CurveType::base_field_type::value_type left_side = y * y;
                typename CurveType::base_field_type::value_type right_side = x * x * x;
                right_side += 5;
                if (left_side != right_side) {
                    x = 0;
                    y = 0;
                }
            }

            template<typename CurveType>
            typename CurveType::template g1_type<nil::crypto3::algebra::curves::coordinates::affine>::value_type
                read_point(boost::json::array point) {
                using base_field_type = typename CurveType::base_field_type;
                auto x = boost::json::value_to<typename base_field_type::value_type>(point[0]);
                auto y = boost::json::value_to<typename base_field_type::value_type>(point[1]);
                check_coord<CurveType>(x, y);

                return {x, y};
            }

        }    // namespace mina_state
    }        // namespace proof_generator
}    // namespace nil

#endif    // PROOF_GENERATOR_CIRCUITS_MINA_STATE_DESERIALIZATION_POINT_HPP