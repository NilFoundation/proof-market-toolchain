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

#ifndef PROOF_GENERATOR_CIRCUITS_MINA_STATE_DESERIALIZATION_MINA_PROOF_HPP
#define PROOF_GENERATOR_CIRCUITS_MINA_STATE_DESERIALIZATION_MINA_PROOF_HPP

#include <nil/crypto3/algebra/curves/pallas.hpp>
#include <nil/crypto3/algebra/fields/arithmetic_params/pallas.hpp>

#include <nil/crypto3/zk/snark/systems/plonk/pickles/verifier_index.hpp>

#include <nil/marshalling/endianness.hpp>
#include <nil/crypto3/marshalling/zk/types/placeholder/proof.hpp>
#include <nil/marshalling/status_type.hpp>
#include <nil/marshalling/field_type.hpp>

#include <nil/proof-generator/circuits/mina-state/deserialization/fp_element.hpp>
#include <nil/proof-generator/circuits/mina-state/deserialization/point.hpp>

namespace nil {
    namespace proof_generator {
        namespace mina_state {

            nil::crypto3::zk::snark::proof_type<curve_type> make_proof(boost::json::value public_input) {
                using curve_type = typename nil::crypto3::algebra::curves::pallas;
                using scalar_field_type = typename curve_type::scalar_field_type;
                using base_field_type = typename curve_type::base_field_type;

                typename nil::crypto3::zk::snark::proof_type<curve_type> proof;

                boost::json::array best_chain = public_input.at("data").at("bestChain").as_array();
                boost::json::value proof_jv = best_chain[0].at("protocolStateProof").at("json").at("proof");

                boost::json::array w_comm_jv = proof_jv.at("messages").at("w_comm").as_array();
                for (std::size_t i = 0; i < w_comm_jv.size(); i++) {
                    boost::json::array unshifted = w_comm_jv[i].as_array();
                    for (std::size_t j = 0; j < unshifted.size(); j++) {
                        boost::json::array point = unshifted[j].as_array();
                        proof.commitments.w_comm[i].unshifted.push_back(read_point<curve_type>(point));
                    }
                }

                boost::json::array z_comm_jv = proof_jv.at("messages").at("z_comm").as_array();
                for (std::size_t i = 0; i < z_comm_jv.size(); i++) {
                    boost::json::array point = z_comm_jv[i].as_array();
                    proof.commitments.z_comm.unshifted.push_back(read_point<curve_type>(point));
                }

                boost::json::array t_comm_jv = proof_jv.at("messages").at("t_comm").as_array();
                for (std::size_t i = 0; i < t_comm_jv.size(); i++) {
                    boost::json::array point = t_comm_jv[i].as_array();
                    proof.commitments.t_comm.unshifted.push_back(read_point<curve_type>(point));
                }

                // TODO: lookup

                boost::json::value openings_proof_jv = proof_jv.at("openings").at("proof");
                boost::json::array lr_jv = openings_proof_jv.at("lr").as_array();
                for (std::size_t i = 0; i < lr_jv.size(); i++) {
                    boost::json::array points = lr_jv[i].as_array();
                    boost::json::array point_l = points[0].as_array();
                    boost::json::array point_r = points[1].as_array();
                    proof.proof.lr.push_back({read_point<curve_type>(point_l), read_point<curve_type>(point_r)});
                }

                boost::json::array delta_jv = openings_proof_jv.at("delta").as_array();
                proof.proof.delta = read_point<curve_type>(delta_jv);

                boost::json::array sg_jv = openings_proof_jv.at("challenge_polynomial_commitment").as_array();
                proof.proof.sg = read_point<curve_type>(sg_jv);

                proof.proof.z1 = boost::json::value_to<scalar_field_type::value_type>(openings_proof_jv.at("z_1"));
                proof.proof.z2 = boost::json::value_to<scalar_field_type::value_type>(openings_proof_jv.at("z_2"));

                boost::json::object evals_jv = proof_jv.at("openings").at("evals").as_object();

                boost::json::array w_jv = evals_jv.at("w").as_array();
                for (std::size_t j = 0; j < w_jv.size(); j++) {
                    boost::json::array eval_at_point_jv_arr = w_jv[j].as_array();
                    for (std::size_t i = 0; i < eval_at_point_jv_arr.size(); i++) {
                        boost::json::array eval_at_point_jv = eval_at_point_jv_arr[i].as_array();
                        for (std::size_t k = 0; k < eval_at_point_jv.size(); k++) {
                            proof.evals[i].w[j].push_back(
                                boost::json::value_to<scalar_field_type::value_type>(eval_at_point_jv[k]));
                        }
                    }
                }

                boost::json::array z_jv_arr = evals_jv.at("z").as_array();
                for (std::size_t i = 0; i < z_jv_arr.size(); i++) {
                    boost::json::array z_jv = z_jv_arr[i].as_array();
                    for (std::size_t j = 0; j < z_jv.size(); j++) {
                        proof.evals[i].z.push_back(boost::json::value_to<scalar_field_type::value_type>(z_jv[j]));
                    }
                }
                boost::json::array s_jv = evals_jv.at("s").as_array();
                for (std::size_t j = 0; j < s_jv.size(); j++) {
                    boost::json::array eval_at_point_jv_arr = s_jv[j].as_array();
                    for (std::size_t i = 0; i < eval_at_point_jv_arr.size(); i++) {
                        boost::json::array eval_at_point_jv = eval_at_point_jv_arr[i].as_array();
                        for (std::size_t k = 0; k < eval_at_point_jv.size(); k++) {
                            proof.evals[i].s[j].push_back(
                                boost::json::value_to<scalar_field_type::value_type>(eval_at_point_jv[k]));
                        }
                    }
                }

                boost::json::array generic_selector_jv_arr = evals_jv.at("generic_selector").as_array();
                for (std::size_t i = 0; i < z_jv_arr.size(); i++) {
                    boost::json::array generic_selector_jv = generic_selector_jv_arr[i].as_array();
                    for (std::size_t j = 0; j < generic_selector_jv.size(); j++) {
                        proof.evals[i].generic_selector.push_back(
                            boost::json::value_to<scalar_field_type::value_type>(generic_selector_jv[j]));
                    }
                }

                boost::json::array poseidon_selector_jv_arr = evals_jv.at("poseidon_selector").as_array();
                for (std::size_t i = 0; i < z_jv_arr.size(); i++) {
                    boost::json::array poseidon_selector_jv = poseidon_selector_jv_arr[i].as_array();
                    for (std::size_t j = 0; j < poseidon_selector_jv.size(); j++) {
                        proof.evals[i].poseidon_selector.push_back(
                            boost::json::value_to<scalar_field_type::value_type>(poseidon_selector_jv[j]));
                    }
                }

                std::size_t i = 0;
                proof.ft_eval1 =
                    boost::json::value_to<scalar_field_type::value_type>(proof_jv.at("openings").at("ft_eval1"));

                // TODO: public input
                // TODO: prev challenges
                return proof;
            }

        }    // namespace mina_state
    }        // namespace proof_generator
}    // namespace nil

#endif    // PROOF_GENERATOR_CIRCUITS_MINA_STATE_DESERIALIZATION_MINA_PROOF_HPP