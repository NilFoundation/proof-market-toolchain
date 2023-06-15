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

#ifndef PROOF_GENERATOR_CIRCUITS_MINA_STATE_DESERIALIZATION_VERIFIER_INDEX_HPP
#define PROOF_GENERATOR_CIRCUITS_MINA_STATE_DESERIALIZATION_VERIFIER_INDEX_HPP

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

            using curve_type = nil::crypto3::algebra::curves::pallas;
            using pallas_verifier_index_type = nil::crypto3::zk::snark::verifier_index<
                curve_type,
                nil::crypto3::zk::snark::arithmetic_sponge_params<curve_type::scalar_field_type::value_type>,
                nil::crypto3::zk::snark::arithmetic_sponge_params<curve_type::base_field_type::value_type>,
                nil::crypto3::zk::snark::kimchi_constant::COLUMNS,
                nil::crypto3::zk::snark::kimchi_constant::PERMUTES>;

            void read_commitments(boost::json::value vk, pallas_verifier_index_type &ver_index) {
                boost::json::array sigma_comm_jv = vk.at("commitments").at("sigma_comm").as_array();
                for (std::size_t i = 0; i < sigma_comm_jv.size(); ++i) {
                    boost::json::array point = sigma_comm_jv[i].as_array();
                    ver_index.sigma_comm[i].unshifted.emplace_back(read_point<curve_type>(point));
                }

                boost::json::array coefficients_comm_jv = vk.at("commitments").at("coefficients_comm").as_array();
                for (std::size_t i = 0; i < coefficients_comm_jv.size(); ++i) {
                    boost::json::array point = coefficients_comm_jv[i].as_array();
                    ver_index.coefficients_comm[i].unshifted.emplace_back(read_point<curve_type>(point));
                }

                boost::json::array generic_comm_jv = vk.at("commitments").at("generic_comm").as_array();
                ver_index.generic_comm.unshifted.emplace_back(read_point<curve_type>(generic_comm_jv));

                boost::json::array psm_comm_jv = vk.at("commitments").at("psm_comm").as_array();
                ver_index.psm_comm.unshifted.emplace_back(read_point<curve_type>(psm_comm_jv));

                boost::json::array complete_add_comm_jv = vk.at("commitments").at("complete_add_comm").as_array();
                ver_index.complete_add_comm.unshifted.emplace_back(read_point<curve_type>(complete_add_comm_jv));

                boost::json::array mul_comm_jv = vk.at("commitments").at("mul_comm").as_array();
                ver_index.mul_comm.unshifted.emplace_back(read_point<curve_type>(mul_comm_jv));

                boost::json::array emul_comm_jv = vk.at("commitments").at("emul_comm").as_array();
                ver_index.emul_comm.unshifted.emplace_back(read_point<curve_type>(emul_comm_jv));

                boost::json::array endomul_scalar_comm_jv = vk.at("commitments").at("endomul_scalar_comm").as_array();
                ver_index.endomul_scalar_comm.unshifted.emplace_back(read_point<curve_type>(endomul_scalar_comm_jv));

                // TODO: chacha_comm
            }

            pallas_verifier_index_type make_verifier_index(boost::json::value public_input,
                                                           boost::json::value const_input) {
                using curve_type = typename nil::crypto3::algebra::curves::pallas;
                using scalar_field_type = typename curve_type::scalar_field_type;
                using base_field_type = typename curve_type::base_field_type;

                pallas_verifier_index_type ver_index;

                // TODO Is it right? Is it a good way to set domain generator?
                // We need to assert, need to check that the input is indeed the root of unity
                boost::json::value verify_index_input = const_input.at("kimchi_const").at("verify_index");
                boost::json::value domain = verify_index_input.at("domain");
                ver_index.domain.omega = boost::json::value_to<scalar_field_type::value_type>(domain.at("group_gen"));
                std::uint32_t d_size = boost::json::value_to<std::uint32_t>(domain.at("log_size_of_group"));
                ver_index.domain = nil::crypto3::math::basic_radix2_domain<scalar_field_type>(d_size + 1);

                boost::json::value vk = public_input.at("data").at("blockchainVerificationKey");
                ver_index.max_poly_size = boost::json::value_to<std::size_t>(vk.at("index").at("max_poly_size"));
                ver_index.max_quot_size = boost::json::value_to<std::size_t>(vk.at("index").at("max_poly_size"));

                // TODO srs for ver_index

                read_commitments(vk, ver_index);

                // TODO: shifts

                // Polynomial in coefficients form
                // Const
                ver_index.zkpm = {0x2C46205451F6C3BBEA4BABACBEE609ECF1039A903C42BFF639EDC5BA33356332_cppui256,
                                  0x1764D9CB4C64EBA9A150920807637D458919CB6948821F4D15EB1994EADF9CE3_cppui256,
                                  0x0140117C8BBC4CE4644A58F7007148577782213065BB9699BF5C391FBE1B3E6D_cppui256,
                                  0x0000000000000000000000000000000000000000000000000000000000000001_cppui256};

                ver_index.w = boost::json::value_to<scalar_field_type::value_type>(verify_index_input.at("w"));
                ver_index.endo = boost::json::value_to<scalar_field_type::value_type>(verify_index_input.at("endo"));

                // TODO: lookup_index
                ver_index.powers_of_alpha.next_power = 24;

                boost::json::array fr_round_constants_jv =
                    verify_index_input.at("fr_sponge_params").at("round_constants").as_array();
                ver_index.fr_sponge_params.round_constants.resize(fr_round_constants_jv.size());
                for (std::size_t i = 0; i < fr_round_constants_jv.size(); i++) {
                    boost::json::array row_jv = fr_round_constants_jv[i].as_array();
                    for (std::size_t j = 0; j < row_jv.size(); j++) {
                        ver_index.fr_sponge_params.round_constants[i].push_back(
                            boost::json::value_to<scalar_field_type::value_type>(row_jv[j]));
                    }
                }

                boost::json::array fr_mds_jv = verify_index_input.at("fr_sponge_params").at("mds").as_array();
                for (std::size_t i = 0; i < fr_mds_jv.size(); i++) {
                    boost::json::array row_jv = fr_mds_jv[i].as_array();
                    for (std::size_t j = 0; j < row_jv.size(); j++) {
                        ver_index.fr_sponge_params.mds[i][j] =
                            boost::json::value_to<scalar_field_type::value_type>(row_jv[j]);
                    }
                }

                boost::json::array fq_round_constants_jv =
                    verify_index_input.at("fq_sponge_params").at("round_constants").as_array();
                ver_index.fq_sponge_params.round_constants.resize(fq_round_constants_jv.size());
                for (std::size_t i = 0; i < fq_round_constants_jv.size(); i++) {
                    boost::json::array row_jv = fq_round_constants_jv[i].as_array();
                    for (std::size_t j = 0; j < row_jv.size(); j++) {
                        ver_index.fq_sponge_params.round_constants[i].push_back(
                            boost::json::value_to<base_field_type::value_type>(row_jv[j]));
                    }
                }

                boost::json::array fq_mds_jv = verify_index_input.at("fq_sponge_params").at("mds").as_array();
                for (std::size_t i = 0; i < fq_mds_jv.size(); i++) {
                    boost::json::array row_jv = fq_mds_jv[i].as_array();
                    for (std::size_t j = 0; j < row_jv.size(); j++) {
                        ver_index.fq_sponge_params.mds[i][j] =
                            boost::json::value_to<base_field_type::value_type>(row_jv[j]);
                    }
                }

                // TODO: Add assertions about right size of
                //      fr_sponge_params.mds,
                //      fr_sponge_params.round_constants,

                return ver_index;
            }

        }    // namespace mina_state
    }        // namespace proof_generator
}    // namespace nil

#endif    // PROOF_GENERATOR_CIRCUITS_MINA_STATE_DESERIALIZATION_VERIFIER_INDEX_HPP