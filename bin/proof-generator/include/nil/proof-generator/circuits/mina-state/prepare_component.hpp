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

#ifndef PROOF_GENERATOR_CIRCUITS_MINA_STATE_PREPARE_COMPONENT_HPP
#define PROOF_GENERATOR_CIRCUITS_MINA_STATE_PREPARE_COMPONENT_HPP

#include <nil/crypto3/math/algorithms/calculate_domain_set.hpp>

#include <nil/crypto3/algebra/curves/pallas.hpp>

#include <nil/blueprint_mc/assignment/plonk.hpp>
#include <nil/blueprint_mc/components/systems/snark/plonk/kimchi/proof_system/circuit_description.hpp>

#include <nil/crypto3/zk/snark/arithmetization/plonk/params.hpp>

namespace nil {
    namespace proof_generator {
        namespace mina_state {

            inline std::vector<std::size_t> generate_step_list(const std::size_t r, const int max_step) {
                std::vector<std::size_t> step_list;
                std::size_t steps_sum = 0;
                while (steps_sum != r) {
                    if (r - steps_sum <= max_step) {
                        while (r - steps_sum != 1) {
                            step_list.emplace_back(r - steps_sum - 1);
                            steps_sum += step_list.back();
                        }
                        step_list.emplace_back(1);
                        steps_sum += step_list.back();
                    } else {
                        step_list.emplace_back(max_step);
                        steps_sum += step_list.back();
                    }
                }
                return step_list;
            }

            template<typename fri_type, typename FieldType>
            typename fri_type::params_type create_fri_params(std::size_t degree_log, std::size_t max_step) {
                typename fri_type::params_type params;

                constexpr std::size_t expand_factor = 0;
                std::size_t r = degree_log - 1;

                std::vector<std::shared_ptr<nil::crypto3::math::evaluation_domain<FieldType>>> domain_set =
                    nil::crypto3::math::calculate_domain_set<FieldType>(degree_log + expand_factor, r);

                params.r = r;
                params.D = domain_set;
                params.max_degree = (1 << degree_log) - 1;

                params.step_list = generate_step_list(r, max_step);

                return params;
            }

            template<typename ComponentType, typename BlueprintFieldType, typename ArithmetizationParams, typename Hash,
                     std::size_t Lambda, typename FunctorResultCheck, typename PublicInput,
                     typename std::enable_if<
                         std::is_same<typename BlueprintFieldType::value_type,
                                      typename std::iterator_traits<typename PublicInput::iterator>::value_type>::value,
                         bool>::type = true>
            auto prepare_component(typename ComponentType::params_type params, const PublicInput &public_input,
                                   const std::size_t max_step, const FunctorResultCheck &result_check) {

                using ArithmetizationType =
                    nil::crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>;
                using component_type = ComponentType;

                nil::crypto3::zk::snark::plonk_table_description<BlueprintFieldType, ArithmetizationParams> desc;

                nil::blueprint_mc::blueprint<ArithmetizationType> bp(desc);
                nil::blueprint_mc::blueprint_private_assignment_table<ArithmetizationType> private_assignment(desc);
                nil::blueprint_mc::blueprint_public_assignment_table<ArithmetizationType> public_assignment(desc);
                nil::blueprint_mc::blueprint_assignment_table<ArithmetizationType> assignment_bp(private_assignment,
                                                                                                 public_assignment);

                std::size_t start_row = nil::blueprint_mc::components::allocate<component_type>(bp);
                if (public_input.size() > component_type::rows_amount) {
                    bp.allocate_rows(public_input.size() - component_type::rows_amount);
                }

                for (std::size_t i = 0; i < public_input.size(); i++) {
                    auto allocated_pi = assignment_bp.allocate_public_input(public_input[i]);
                }

                nil::blueprint_mc::components::generate_circuit<component_type>(bp, public_assignment, params,
                                                                                start_row);
                typename component_type::result_type component_result =
                    component_type::generate_assignments(assignment_bp, params, start_row);

                result_check(assignment_bp, component_result);

                assignment_bp.padding();

                nil::crypto3::zk::snark::plonk_assignment_table<BlueprintFieldType, ArithmetizationParams> assignments(
                    private_assignment, public_assignment);

                using placeholder_params =
                    nil::crypto3::zk::snark::placeholder_params<BlueprintFieldType, ArithmetizationParams, Hash, Hash,
                                                                Lambda>;
                using types =
                    nil::crypto3::zk::snark::detail::placeholder_policy<BlueprintFieldType, placeholder_params>;

                using fri_type = typename nil::crypto3::zk::commitments::fri<
                    BlueprintFieldType, typename placeholder_params::merkle_hash_type,
                    typename placeholder_params::transcript_hash_type, 1, 2, 4>;

                std::size_t table_rows_log = std::ceil(std::log2(desc.rows_amount));

                typename fri_type::params_type fri_params =
                    create_fri_params<fri_type, BlueprintFieldType>(table_rows_log, max_step);

                std::size_t permutation_size = desc.witness_columns + desc.public_input_columns + desc.constant_columns;

                typename nil::crypto3::zk::snark::placeholder_public_preprocessor<
                    BlueprintFieldType, placeholder_params>::preprocessed_data_type public_preprocessed_data =
                    nil::crypto3::zk::snark::placeholder_public_preprocessor<
                        BlueprintFieldType, placeholder_params>::process(bp, public_assignment, desc, fri_params,
                                                                         permutation_size);
                typename nil::crypto3::zk::snark::placeholder_private_preprocessor<
                    BlueprintFieldType, placeholder_params>::preprocessed_data_type private_preprocessed_data =
                    nil::crypto3::zk::snark::placeholder_private_preprocessor<
                        BlueprintFieldType, placeholder_params>::process(bp, private_assignment, desc, fri_params);

                return std::make_tuple(desc, bp, fri_params, assignments, public_preprocessed_data,
                                       private_preprocessed_data);
            }
        }    // namespace mina_state
    }        // namespace proof_generator
}    // namespace nil

#endif    // PROOF_GENERATOR_CIRCUITS_MINA_STATE_PREPARE_COMPONENT_HPP