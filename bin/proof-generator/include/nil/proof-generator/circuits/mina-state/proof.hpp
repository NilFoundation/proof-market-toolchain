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

#ifndef PROOF_GENERATOR_CIRCUITS_MINA_STATE_PROOF_HPP
#define PROOF_GENERATOR_CIRCUITS_MINA_STATE_PROOF_HPP

#include <boost/json/src.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/optional.hpp>

#ifndef __EMSCRIPTEN__
#include <boost/filesystem.hpp>
#include <boost/filesystem/string_file.hpp>
#include <boost/program_options.hpp>
#endif

#include <nil/crypto3/algebra/curves/pallas.hpp>

#include <nil/crypto3/zk/snark/systems/plonk/pickles/proof.hpp>
#include <nil/crypto3/zk/snark/systems/plonk/pickles/verifier_index.hpp>

#include <nil/proof-generator/circuits/mina-state/deserialization/verifier_index.hpp>
#include <nil/proof-generator/circuits/mina-state/deserialization/mina_proof.hpp>
#include <nil/proof-generator/circuits/mina-state/proof_scalar.hpp>
#include <nil/proof-generator/circuits/mina-state/proof_base.hpp>

namespace nil {
    namespace proof_generator {
        namespace mina_state {

            void concatenate_proofs(std::string output_path) {
                std::string output_path_scalar = output_path + "_scalar";
                std::string output_path_base = output_path + "_base";
                std::string output_path_full = output_path;
                std::ifstream file_scalar;
                file_scalar.open(output_path_scalar);
                std::ifstream file_base;
                file_base.open(output_path_base);
                std::ofstream file_full;
                file_full.open(output_path_full);

                file_scalar.ignore(2);

                std::string line;

                while (getline(file_base, line)) {
                    file_full << line;
                }

                while (getline(file_scalar, line)) {
                    file_full << line;
                }

                file_base.close();
                file_scalar.close();

                file_full.close();
            }

            template<typename VerifierIndexType, std::size_t EvalRoundsScalar, std::size_t EvalRoundsBase>
            void generate_proof_heterogenous(
                nil::crypto3::zk::snark::proof_type<nil::crypto3::algebra::curves::pallas> &pickles_proof,
                VerifierIndexType &pickles_index, const std::size_t fri_max_step, std::string output_path) {

                std::cout << "generating mina-state proof..." << std::endl;
                generate_proof_scalar<VerifierIndexType, EvalRoundsScalar>(pickles_proof, pickles_index, fri_max_step,
                                                                           output_path);
                generate_proof_base<VerifierIndexType, EvalRoundsBase>(pickles_proof, pickles_index, fri_max_step,
                                                                       output_path);
                concatenate_proofs(output_path);
                std::cout << "mina-state proof is generated" << std::endl;
            }

            void proof_new(boost::json::value jv_pickles_constants, boost::json::value jv_public_input,
                           std::string output_file) {
                using curve_type = nil::crypto3::algebra::curves::pallas;
                using pallas_verifier_index_type = nil::crypto3::zk::snark::verifier_index<
                    curve_type,
                    nil::crypto3::zk::snark::arithmetic_sponge_params<curve_type::scalar_field_type::value_type>,
                    nil::crypto3::zk::snark::arithmetic_sponge_params<curve_type::base_field_type::value_type>,
                    nil::crypto3::zk::snark::kimchi_constant::COLUMNS,
                    nil::crypto3::zk::snark::kimchi_constant::PERMUTES>;

                pallas_verifier_index_type ver_index = make_verifier_index(jv_public_input, jv_pickles_constants);
                nil::crypto3::zk::snark::proof_type<nil::crypto3::algebra::curves::pallas> proof =
                    make_proof(jv_public_input);

                constexpr const std::size_t eval_rounds_scalar = 8;
                constexpr const std::size_t eval_rounds_base = 8;
                constexpr const std::size_t fri_max_step = 1;

                generate_proof_heterogenous<pallas_verifier_index_type, eval_rounds_scalar, eval_rounds_base>(
                    proof, ver_index, fri_max_step, output_file);
            }
        }    // namespace mina_state
    }        // namespace proof_generator
}    // namespace nil

#endif    // PROOF_GENERATOR_CIRCUITS_MINA_STATE_PROOF_HPP