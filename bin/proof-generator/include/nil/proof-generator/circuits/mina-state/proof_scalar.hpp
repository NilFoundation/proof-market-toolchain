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

#ifndef PROOF_GENERATOR_CIRCUITS_MINA_STATE_PROOF_SCALAR_HPP
#define PROOF_GENERATOR_CIRCUITS_MINA_STATE_PROOF_SCALAR_HPP

#include <nil/crypto3/algebra/curves/pallas.hpp>

#include <nil/blueprint_mc/assignment/plonk.hpp>
#include <nil/blueprint_mc/components/systems/snark/plonk/kimchi/verify_scalar.hpp>
#include <nil/blueprint_mc/components/systems/snark/plonk/kimchi/proof_system/circuit_description.hpp>

#include <nil/crypto3/zk/snark/arithmetization/plonk/params.hpp>
#include <nil/crypto3/zk/snark/systems/plonk/placeholder/preprocessor.hpp>
#include <nil/crypto3/zk/snark/systems/plonk/placeholder/prover.hpp>
#include <nil/crypto3/zk/snark/systems/plonk/placeholder/verifier.hpp>
#include <nil/crypto3/zk/snark/systems/plonk/placeholder/params.hpp>

#include <nil/proof-generator/circuits/mina-state/index_terms.hpp>
#include <nil/proof-generator/circuits/mina-state/prepare_proof.hpp>
#include <nil/proof-generator/circuits/mina-state/prepare_index.hpp>
#include <nil/proof-generator/circuits/mina-state/prepare_component.hpp>

#include <nil/proof-generator/detail/utils.hpp>

namespace nil {
    namespace proof_generator {
        namespace mina_state {

            template<typename VerifierIndexType, std::size_t EvalRounds>
            void generate_proof_scalar(
                nil::crypto3::zk::snark::proof_type<nil::crypto3::algebra::curves::pallas> &pickles_proof,
                VerifierIndexType &pickles_index, const std::size_t fri_max_step, std::string output_path) {
                using curve_type = nil::crypto3::algebra::curves::pallas;
                using BlueprintFieldType = typename curve_type::scalar_field_type;
                constexpr std::size_t WitnessColumns = 15;
                constexpr std::size_t PublicInputColumns = 1;
                constexpr std::size_t ConstantColumns = 1;
                constexpr std::size_t SelectorColumns = 30;
                using ArithmetizationParams =
                    nil::crypto3::zk::snark::plonk_arithmetization_params<WitnessColumns, PublicInputColumns,
                                                                          ConstantColumns, SelectorColumns>;
                using ArithmetizationType =
                    nil::crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>;
                using AssignmentType = nil::blueprint_mc::blueprint_assignment_table<ArithmetizationType>;
                using hash_type = nil::crypto3::hashes::keccak_1600<256>;
                constexpr std::size_t Lambda = 1;

                using var = nil::crypto3::zk::snark::plonk_variable<BlueprintFieldType>;

                constexpr static std::size_t public_input_size = 1;
                constexpr static std::size_t max_poly_size = 1 << EvalRounds;
                constexpr static std::size_t srs_len = max_poly_size;
                constexpr static std::size_t eval_rounds = EvalRounds;

                constexpr static std::size_t witness_columns = 15;
                constexpr static std::size_t perm_size = 7;
                constexpr static std::size_t lookup_table_size = 0;

                constexpr static std::size_t batch_size = 1;

                constexpr static const std::size_t prev_chal_size = 0;

                using commitment_params =
                    nil::blueprint_mc::components::kimchi_commitment_params_type<eval_rounds, max_poly_size, srs_len>;
                using index_terms_list = nil::blueprint_mc::components::index_terms_scalars_list<ArithmetizationType>;
                using circuit_description =
                    nil::blueprint_mc::components::kimchi_circuit_description<index_terms_list, witness_columns,
                                                                              perm_size>;
                using kimchi_params = nil::blueprint_mc::components::kimchi_params_type<
                    curve_type, commitment_params, circuit_description, public_input_size, prev_chal_size>;

                using component_type =
                    nil::blueprint_mc::components::verify_scalar<ArithmetizationType, curve_type, kimchi_params,
                                                                 commitment_params, batch_size, 0, 1, 2, 3, 4, 5, 6, 7,
                                                                 8, 9, 10, 11, 12, 13, 14>;

                using fq_output_type =
                    typename nil::blueprint_mc::components::binding<ArithmetizationType, BlueprintFieldType,
                                                                    kimchi_params>::fq_sponge_output;

                using fr_data_type =
                    typename nil::blueprint_mc::components::binding<ArithmetizationType, BlueprintFieldType,
                                                                    kimchi_params>::template fr_data<var, batch_size>;

                using fq_data_type =
                    typename nil::blueprint_mc::components::binding<ArithmetizationType, BlueprintFieldType,
                                                                    kimchi_params>::template fq_data<var>;

                std::vector<typename BlueprintFieldType::value_type> public_input = {0};

                std::array<
                    nil::blueprint_mc::components::kimchi_proof_scalar<BlueprintFieldType, kimchi_params, eval_rounds>,
                    batch_size>
                    proofs;

                for (std::size_t batch_id = 0; batch_id < batch_size; batch_id++) {
                    nil::blueprint_mc::components::kimchi_proof_scalar<BlueprintFieldType, kimchi_params, eval_rounds>
                        proof;

                    prepare_proof_scalar<curve_type, BlueprintFieldType, kimchi_params, eval_rounds>(
                        pickles_proof, proof, public_input);

                    proofs[batch_id] = proof;
                }

                nil::blueprint_mc::components::kimchi_verifier_index_scalar<BlueprintFieldType> verifier_index;
                prepare_index_scalar<VerifierIndexType, curve_type, BlueprintFieldType, kimchi_params>(
                    pickles_index, verifier_index, public_input);
                verifier_index.domain_size = max_poly_size;

                using fq_output_type =
                    typename nil::blueprint_mc::components::binding<ArithmetizationType, BlueprintFieldType,
                                                                    kimchi_params>::fq_sponge_output;

                fr_data_type fr_data_public;
                fq_data_type fq_data_public;
                std::array<fq_output_type, batch_size> fq_outputs;

                typename component_type::params_type params = {fr_data_public, fq_data_public, verifier_index, proofs,
                                                               fq_outputs};

                auto result_check = [](AssignmentType &assignment, typename component_type::result_type &real_res) {};

                using placeholder_params =
                    nil::crypto3::zk::snark::placeholder_params<BlueprintFieldType, ArithmetizationParams, hash_type,
                                                                hash_type, Lambda>;

                auto [desc, bp, fri_params, assignments, public_preprocessed_data, private_preprocessed_data] =
                    prepare_component<component_type, BlueprintFieldType, ArithmetizationParams, hash_type, Lambda>(
                        params, public_input, fri_max_step, result_check);

                auto proof =
                    nil::crypto3::zk::snark::placeholder_prover<BlueprintFieldType, placeholder_params>::process(
                        public_preprocessed_data, private_preprocessed_data, desc, bp, assignments, fri_params);

                bool verifier_res =
                    nil::crypto3::zk::snark::placeholder_verifier<BlueprintFieldType, placeholder_params>::process(
                        public_preprocessed_data, proof, bp, fri_params);

                if (verifier_res) {
                    std::cout << "Inner verification passed" << std::endl;
                } else {
                    std::cout << "Inner verification failed" << std::endl;
                }

                std::string output_path_full = output_path + "_scalar";
                proof_print<nil::marshalling::option::big_endian>(proof, output_path_full);
            }
        }    // namespace mina_state
    }        // namespace proof_generator
}    // namespace nil

#endif    // PROOF_GENERATOR_CIRCUITS_MINA_STATE_PROOF_SCALAR_HPP