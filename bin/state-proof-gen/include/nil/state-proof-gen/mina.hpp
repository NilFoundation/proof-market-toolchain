//---------------------------------------------------------------------------//
// Copyright (c) 2018-2021 Mikhail Komarov <nemo@nil.foundation>
// Copyright (c) 2022 Aleksei Moskvin <alalmoskvin@nil.foundation>
// Copyright (c) 2022 Ilia Shirobokov <i.shirobokov@nil.foundation>
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

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/optional.hpp>

#ifndef __EMSCRIPTEN__
#include <boost/filesystem.hpp>
#include <boost/filesystem/string_file.hpp>
#include <boost/program_options.hpp>
#endif

#include <nil/crypto3/algebra/curves/pallas.hpp>
#include <nil/crypto3/algebra/fields/arithmetic_params/pallas.hpp>

#include <nil/crypto3/zk/blueprint/plonk.hpp>
#include <nil/crypto3/zk/assignment/plonk.hpp>
#include <nil/crypto3/zk/components/systems/snark/plonk/kimchi/verifier_base_field.hpp>
#include <nil/crypto3/zk/components/systems/snark/plonk/kimchi/verify_scalar.hpp>
#include <nil/crypto3/zk/components/systems/snark/plonk/kimchi/proof_system/circuit_description.hpp>

#include <nil/crypto3/math/algorithms/calculate_domain_set.hpp>

#include <nil/crypto3/hash/algorithm/hash.hpp>
#include <nil/crypto3/hash/keccak.hpp>

#include <nil/crypto3/pubkey/algorithm/sign.hpp>
#include <nil/crypto3/pubkey/eddsa.hpp>

#include <nil/crypto3/zk/commitments/type_traits.hpp>
#include <nil/crypto3/zk/snark/arithmetization/plonk/params.hpp>
#include <nil/crypto3/zk/snark/systems/plonk/pickles/proof.hpp>
#include <nil/crypto3/zk/snark/systems/plonk/pickles/verifier_index.hpp>
#include <nil/crypto3/zk/snark/systems/plonk/placeholder/preprocessor.hpp>
#include <nil/crypto3/zk/snark/systems/plonk/placeholder/prover.hpp>
#include <nil/crypto3/zk/snark/systems/plonk/placeholder/verifier.hpp>
#include <nil/crypto3/zk/snark/systems/plonk/placeholder/params.hpp>

#include <nil/marshalling/endianness.hpp>
#include <nil/crypto3/marshalling/zk/types/placeholder/proof.hpp>
#include <nil/marshalling/status_type.hpp>
#include <nil/marshalling/field_type.hpp>

#include <nil/state-proof-gen/ec_index_terms.hpp>

namespace nil {
    namespace crypto3 {
        namespace mina_state_proof {

            using curve_type = nil::crypto3::algebra::curves::pallas;
            using pallas_verifier_index_type = zk::snark::verifier_index<
                curve_type, nil::crypto3::zk::snark::arithmetic_sponge_params<curve_type::scalar_field_type::value_type>,
                nil::crypto3::zk::snark::arithmetic_sponge_params<curve_type::base_field_type::value_type>,
                nil::crypto3::zk::snark::kimchi_constant::COLUMNS, nil::crypto3::zk::snark::kimchi_constant::PERMUTES>;

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

            std::vector<std::shared_ptr<math::evaluation_domain<FieldType>>> domain_set =
                math::calculate_domain_set<FieldType>(degree_log + expand_factor, r);

            params.r = r;
            params.D = domain_set;
            params.max_degree = (1 << degree_log) - 1;

            params.step_list = generate_step_list(r, max_step);

            return params;
        }

            template<typename Iterator>
            multiprecision::cpp_int get_cppui256(Iterator it) {
                BOOST_ASSERT(it->second.template get_value<std::string>() != "");
                return multiprecision::cpp_int(it->second.template get_value<std::string>());
            }

            template<typename CurveType>
            void check_coord(typename CurveType::base_field_type::value_type &x, typename CurveType::base_field_type::value_type &y) {
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

            zk::snark::proof_type<curve_type> make_proof(boost::json::value public_input) {
                using curve_type = typename nil::crypto3::algebra::curves::pallas;
                using scalar_field_type = typename curve_type::scalar_field_type;
                using base_field_type = typename curve_type::base_field_type;

                typename zk::snark::proof_type<curve_type> proof;

                boost::json::array best_chain = public_input.at("data").at("bestChain").as_array();
                boost::json::value proof_jv = best_chain[0].at("protocolStateProof").at("json").at("proof");

                
                boost::json::array w_comm_jv = proof_jv.at("messages").at("w_comm").as_array();
                for (std::size_t i = 0; i < w_comm_jv.size(); i++) {
                    boost::json::array unshifted = w_comm_jv[i].as_array();
                    for (std::size_t j = 0; j < unshifted.size(); j++) {
                        boost::json::array point = unshifted[j].as_array();
                        auto x = boost::json::value_to<base_field_type::value_type>(point[0]);
                        auto y = boost::json::value_to<base_field_type::value_type>(point[1]);
                        check_coord<curve_type>(x, y);
                        proof.commitments.w_comm[i].unshifted.push_back({x, y});
                    }
                }
                
                boost::json::array z_comm_jv = proof_jv.at("messages").at("z_comm").as_array();
                for (std::size_t i = 0; i < z_comm_jv.size(); i++) {
                    boost::json::array point = z_comm_jv[i].as_array();
                    auto x = boost::json::value_to<base_field_type::value_type>(point[0]);
                    auto y = boost::json::value_to<base_field_type::value_type>(point[1]);
                    check_coord<curve_type>(x, y);
                    proof.commitments.z_comm.unshifted.push_back({x, y});
                }

                boost::json::array t_comm_jv = proof_jv.at("messages").at("t_comm").as_array();
                for (std::size_t i = 0; i < t_comm_jv.size(); i++) {
                    boost::json::array point = t_comm_jv[i].as_array();
                    auto x = boost::json::value_to<base_field_type::value_type>(point[0]);
                    auto y = boost::json::value_to<base_field_type::value_type>(point[1]);
                    check_coord<curve_type>(x, y);
                    proof.commitments.t_comm.unshifted.push_back({x, y});
                }

                // TODO: lookup

                boost::json::value openings_proof_jv = proof_jv.at("openings").at("proof");
                boost::json::array lr_jv = openings_proof_jv.at("lr").as_array();
                for (std::size_t i = 0; i < lr_jv.size(); i++) {
                    boost::json::array points = lr_jv[i].as_array();
                    boost::json::array point_l = points[0].as_array();
                    auto x_l = boost::json::value_to<base_field_type::value_type>(point_l[0]);
                    auto y_l = boost::json::value_to<base_field_type::value_type>(point_l[1]);
                    check_coord<curve_type>(x_l, y_l);
                    boost::json::array point_r = points[1].as_array();
                    auto x_r = boost::json::value_to<base_field_type::value_type>(point_r[0]);
                    auto y_r = boost::json::value_to<base_field_type::value_type>(point_r[1]);
                    proof.proof.lr.push_back({{x_l, y_l}, {x_r, y_r}});
                }

                boost::json::array delta_jv = openings_proof_jv.at("delta").as_array();
                auto delta_x = boost::json::value_to<base_field_type::value_type>(delta_jv[0]);
                auto delta_y = boost::json::value_to<base_field_type::value_type>(delta_jv[1]);
                check_coord<curve_type>(delta_x, delta_y);
                proof.proof.delta = {delta_x, delta_y};
                
                boost::json::array sg_jv = openings_proof_jv.at("sg").as_array();
                auto sg_x = boost::json::value_to<base_field_type::value_type>(sg_jv[0]);
                auto sg_y = boost::json::value_to<base_field_type::value_type>(sg_jv[1]);
                check_coord<curve_type>(sg_x, sg_y);
                proof.proof.sg = {sg_x, sg_y};
                
                proof.proof.z1 = boost::json::value_to<scalar_field_type::value_type>(openings_proof_jv.at("z_1"));
                proof.proof.z2 = boost::json::value_to<scalar_field_type::value_type>(openings_proof_jv.at("z_2"));

                boost::json::array evals_jv = proof_jv.at("openings").at("evals").as_array();
                for (std::size_t i = 0; i < evals_jv.size(); i++) {
                    boost::json::array w_jv = evals_jv[i].at("w").as_array();
                    for (std::size_t j = 0; j < w_jv.size(); j++) {
                        boost::json::array eval_at_point_jv = w_jv[j].as_array();
                        for (std::size_t k = 0; k < eval_at_point_jv.size(); k++) {
                            proof.evals[i].w[j].push_back(boost::json::value_to<scalar_field_type::value_type>(eval_at_point_jv[k]));
                        }
                    }

                    boost::json::array z_jv = evals_jv[i].at("z").as_array();
                    for (std::size_t j = 0; j < z_jv.size(); j++) {
                        proof.evals[i].z.push_back(boost::json::value_to<scalar_field_type::value_type>(z_jv[j]));
                    }

                    boost::json::array s_jv = evals_jv[i].at("s").as_array();
                    for (std::size_t j = 0; j < s_jv.size(); j++) {
                        boost::json::array eval_at_point_jv = s_jv[j].as_array();
                        for (std::size_t k = 0; k < eval_at_point_jv.size(); k++) {
                            proof.evals[i].s[j].push_back(boost::json::value_to<scalar_field_type::value_type>(eval_at_point_jv[k]));
                        }
                    }

                    boost::json::array generic_selector_jv = evals_jv[i].at("generic_selector").as_array();
                    for (std::size_t j = 0; j < generic_selector_jv.size(); j++) {
                        proof.evals[i].generic_selector.push_back(boost::json::value_to<scalar_field_type::value_type>(generic_selector_jv[j]));
                    }

                    boost::json::array poseidon_selector_jv = evals_jv[i].at("poseidon_selector").as_array();
                    for (std::size_t j = 0; j < poseidon_selector_jv.size(); j++) {
                        proof.evals[i].poseidon_selector.push_back(boost::json::value_to<scalar_field_type::value_type>(poseidon_selector_jv[j]));
                    }
                }

                std::size_t i = 0;

                proof.ft_eval1 = boost::json::value_to<scalar_field_type::value_type>(proof_jv.at("openings").at("ft_eval1"));
                
                // TODO: public input
                // TODO: prev challenges
                return proof;
            }

            pallas_verifier_index_type make_verify_index(boost::json::value public_input, boost::json::value const_input) {
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
                ver_index.max_quot_size = boost::json::value_to<std::size_t>(vk.at("index").at("max_quot_size"));

                // TODO srs for ver_index

                boost::json::array sigma_comm_jv = vk.at("commitments").at("sigma_comm").as_array();
                for (std::size_t i = 0; i < sigma_comm_jv.size(); ++i) {
                    boost::json::array point = sigma_comm_jv[i].as_array();
                    auto x = boost::json::value_to<base_field_type::value_type>(point[0]);
                    auto y = boost::json::value_to<base_field_type::value_type>(point[1]);
                    check_coord<curve_type>(x, y);
                    ver_index.sigma_comm[i].unshifted.emplace_back(x, y);
                }

                boost::json::array coefficients_comm_jv = vk.at("commitments").at("coefficients_comm").as_array();
                for (std::size_t i = 0; i < coefficients_comm_jv.size(); ++i) {
                    boost::json::array point = coefficients_comm_jv[i].as_array();
                    auto x = boost::json::value_to<base_field_type::value_type>(point[0]);
                    auto y = boost::json::value_to<base_field_type::value_type>(point[1]);
                    check_coord<curve_type>(x, y);
                    ver_index.coefficients_comm[i].unshifted.emplace_back(x, y);
                }

                boost::json::array generic_comm_jv = vk.at("commitments").at("generic_comm").as_array();
                auto x = boost::json::value_to<base_field_type::value_type>(generic_comm_jv[0]);
                auto y = boost::json::value_to<base_field_type::value_type>(generic_comm_jv[1]);
                check_coord<curve_type>(x, y);
                ver_index.generic_comm.unshifted.emplace_back(x, y);

                boost::json::array psm_comm_jv = vk.at("commitments").at("psm_comm").as_array();
                x = boost::json::value_to<base_field_type::value_type>(psm_comm_jv[0]);
                y = boost::json::value_to<base_field_type::value_type>(psm_comm_jv[1]);
                check_coord<curve_type>(x, y);
                ver_index.psm_comm.unshifted.emplace_back(x, y);

                boost::json::array complete_add_comm_jv = vk.at("commitments").at("complete_add_comm").as_array();
                x = boost::json::value_to<base_field_type::value_type>(complete_add_comm_jv[0]);
                y = boost::json::value_to<base_field_type::value_type>(complete_add_comm_jv[1]);
                check_coord<curve_type>(x, y);
                ver_index.complete_add_comm.unshifted.emplace_back(x, y);

                boost::json::array mul_comm_jv = vk.at("commitments").at("mul_comm").as_array();
                x = boost::json::value_to<base_field_type::value_type>(mul_comm_jv[0]);
                y = boost::json::value_to<base_field_type::value_type>(mul_comm_jv[1]);
                check_coord<curve_type>(x, y);
                ver_index.mul_comm.unshifted.emplace_back(x, y);

                boost::json::array emul_comm_jv = vk.at("commitments").at("emul_comm").as_array();
                x = boost::json::value_to<base_field_type::value_type>(emul_comm_jv[0]);
                y = boost::json::value_to<base_field_type::value_type>(emul_comm_jv[1]);
                check_coord<curve_type>(x, y);
                ver_index.emul_comm.unshifted.emplace_back(x, y);

                boost::json::array endomul_scalar_comm_jv = vk.at("commitments").at("endomul_scalar_comm").as_array();
                x = boost::json::value_to<base_field_type::value_type>(endomul_scalar_comm_jv[0]);
                y = boost::json::value_to<base_field_type::value_type>(endomul_scalar_comm_jv[1]);
                check_coord<curve_type>(x, y);
                ver_index.endomul_scalar_comm.unshifted.emplace_back(x, y);

                // TODO: chacha_comm
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

                boost::json::array fr_round_constants_jv = verify_index_input.at("fr_sponge_params").at("round_constants").as_array();
                ver_index.fr_sponge_params.round_constants.resize(fr_round_constants_jv.size());
                for (std::size_t i = 0; i < fr_round_constants_jv.size(); i++) {
                    boost::json::array row_jv = fr_round_constants_jv[i].as_array();
                    for (std::size_t j = 0; j < row_jv.size(); j++) {
                        ver_index.fr_sponge_params.round_constants[i].push_back(boost::json::value_to<scalar_field_type::value_type>(row_jv[j]));
                    }
                }

                boost::json::array fr_mds_jv = verify_index_input.at("fr_sponge_params").at("mds").as_array();
                for (std::size_t i = 0; i < fr_mds_jv.size(); i++) {
                    boost::json::array row_jv = fr_mds_jv[i].as_array();
                    for (std::size_t j = 0; j < row_jv.size(); j++) {
                        ver_index.fr_sponge_params.mds[i][j] = boost::json::value_to<scalar_field_type::value_type>(row_jv[j]);
                    }
                }

                boost::json::array fq_round_constants_jv = verify_index_input.at("fq_sponge_params").at("round_constants").as_array();
                ver_index.fq_sponge_params.round_constants.resize(fq_round_constants_jv.size());
                for (std::size_t i = 0; i < fq_round_constants_jv.size(); i++) {
                    boost::json::array row_jv = fq_round_constants_jv[i].as_array();
                    for (std::size_t j = 0; j < row_jv.size(); j++) {
                        ver_index.fq_sponge_params.round_constants[i].push_back(boost::json::value_to<base_field_type::value_type>(row_jv[j]));
                    }
                }

                boost::json::array fq_mds_jv = verify_index_input.at("fq_sponge_params").at("mds").as_array();
                for (std::size_t i = 0; i < fq_mds_jv.size(); i++) {
                    boost::json::array row_jv = fq_mds_jv[i].as_array();
                    for (std::size_t j = 0; j < row_jv.size(); j++) {
                        ver_index.fq_sponge_params.mds[i][j] = boost::json::value_to<base_field_type::value_type>(row_jv[j]);
                    }
                }

                // TODO: Add assertions about right size of
                //      fr_sponge_params.mds,
                //      fr_sponge_params.round_constants,

                return ver_index;
            }

            template<typename CurveType, typename BlueprintFieldType, typename KimchiParamsType, std::size_t EvalRounds>
            void prepare_proof_scalar(
                zk::snark::proof_type<nil::crypto3::algebra::curves::pallas> &original_proof,
                zk::components::kimchi_proof_scalar<BlueprintFieldType, KimchiParamsType, EvalRounds> &circuit_proof,
                std::vector<typename BlueprintFieldType::value_type> &public_input) {
                using var = zk::snark::plonk_variable<BlueprintFieldType>;

                // eval_proofs
                for (std::size_t point_idx = 0; point_idx < 2; point_idx++) {
                    // w
                    for (std::size_t i = 0; i < KimchiParamsType::witness_columns; i++) {
                        public_input.push_back(original_proof.evals[point_idx].w[i][0]);
                        circuit_proof.proof_evals[point_idx].w[i] =
                            var(0, public_input.size() - 1, false, var::column_type::public_input);
                    }
                    // z
                    public_input.push_back(original_proof.evals[point_idx].z[0]);
                    circuit_proof.proof_evals[point_idx].z = var(0, public_input.size() - 1, false, var::column_type::public_input);
                    // s
                    for (std::size_t i = 0; i < KimchiParamsType::permut_size - 1; i++) {
                        public_input.push_back(original_proof.evals[point_idx].s[i][0]);
                        circuit_proof.proof_evals[point_idx].s[i] =
                            var(0, public_input.size() - 1, false, var::column_type::public_input);
                    }
                    // lookup
                    if (KimchiParamsType::use_lookup) {
                        // TODO
                    }
                    // generic_selector
                    public_input.push_back(original_proof.evals[point_idx].generic_selector[0]);
                    circuit_proof.proof_evals[point_idx].generic_selector =
                        var(0, public_input.size() - 1, false, var::column_type::public_input);
                    // poseidon_selector
                    public_input.push_back(original_proof.evals[point_idx].poseidon_selector[0]);
                    circuit_proof.proof_evals[point_idx].poseidon_selector =
                        var(0, public_input.size() - 1, false, var::column_type::public_input);
                }

                typename BlueprintFieldType::value_type scalar_value = 2;

                circuit_proof.public_input.resize(KimchiParamsType::public_input_size);

                for (std::size_t i = 0; i < KimchiParamsType::public_input_size; i++) {
                    public_input.push_back(scalar_value);
                    circuit_proof.public_input[i] = var(0, public_input.size() - 1, false, var::column_type::public_input);
                }

                for (std::size_t i = 0; i < KimchiParamsType::prev_challenges_size; i++) {
                    for (std::size_t j = 0; j < EvalRounds; j++) {
                        public_input.push_back(original_proof.prev_challenges[i].first[j]);
                        circuit_proof.prev_challenges[i][j] =
                            var(0, public_input.size() - 1, false, var::column_type::public_input);
                    }
                }

                // ft_eval
                public_input.push_back(original_proof.ft_eval1);
                circuit_proof.ft_eval = var(0, public_input.size() - 1, false, var::column_type::public_input);

                // opening proof
                public_input.push_back(scalar_value);
                circuit_proof.opening.z1 = var(0, public_input.size() - 1, false, var::column_type::public_input);
                public_input.push_back(scalar_value);
                circuit_proof.opening.z2 = var(0, public_input.size() - 1, false, var::column_type::public_input);
            }

            template<typename CurveType, typename BlueprintFieldType, typename KimchiParamsType, std::size_t EvalRounds>
            void prepare_proof_base(zk::snark::proof_type<nil::crypto3::algebra::curves::pallas> &original_proof,
                                    zk::components::kimchi_proof_base<BlueprintFieldType, KimchiParamsType> &circuit_proof,
                                    std::vector<typename BlueprintFieldType::value_type> &public_input) {
                using var = zk::snark::plonk_variable<BlueprintFieldType>;
                using kimchi_constants = zk::components::kimchi_inner_constants<KimchiParamsType>;

                // COMMITMENTS
                for (std::size_t i = 0; i < original_proof.commitments.w_comm.size(); i++) {
                    assert(circuit_proof.comm.witness.size() > i);
                    for (std::size_t j = 0; j < original_proof.commitments.w_comm[i].unshifted.size(); j++) {
                        assert(circuit_proof.comm.witness[i].parts.size() > j);
                        public_input.push_back(original_proof.commitments.w_comm[i].unshifted[j].X);
                        circuit_proof.comm.witness[i].parts[j].X =
                            var(0, public_input.size() - 1, false, var::column_type::public_input);
                        public_input.push_back(original_proof.commitments.w_comm[i].unshifted[j].Y);
                        circuit_proof.comm.witness[i].parts[j].Y =
                            var(0, public_input.size() - 1, false, var::column_type::public_input);
                    }
                }

                for (std::size_t j = 0; j < original_proof.commitments.z_comm.unshifted.size(); j++) {
                    assert(circuit_proof.comm.z.parts.size() > j);
                    public_input.push_back(original_proof.commitments.z_comm.unshifted[j].X);
                    circuit_proof.comm.z.parts[j].X = var(0, public_input.size() - 1, false, var::column_type::public_input);
                    public_input.push_back(original_proof.commitments.z_comm.unshifted[j].Y);
                    circuit_proof.comm.z.parts[j].Y = var(0, public_input.size() - 1, false, var::column_type::public_input);
                }

                for (std::size_t j = 0; j < original_proof.commitments.t_comm.unshifted.size(); j++) {
                    assert(circuit_proof.comm.t.parts.size() > j);
                    public_input.push_back(original_proof.commitments.t_comm.unshifted[j].X);
                    circuit_proof.comm.t.parts[j].X = var(0, public_input.size() - 1, false, var::column_type::public_input);
                    public_input.push_back(original_proof.commitments.t_comm.unshifted[j].Y);
                    circuit_proof.comm.t.parts[j].Y = var(0, public_input.size() - 1, false, var::column_type::public_input);
                }

                for (std::size_t i = 0; i < original_proof.commitments.lookup.sorted.size(); i++) {
                    assert(circuit_proof.comm.lookup_sorted.size() > i);
                    for (std::size_t j = 0; j < original_proof.commitments.lookup.sorted[i].unshifted.size(); j++) {
                        assert(circuit_proof.comm.lookup_sorted[i].parts.size() > j);
                        public_input.push_back(original_proof.commitments.lookup.sorted[i].unshifted[j].X);
                        circuit_proof.comm.lookup_sorted[i].parts[j].X =
                            var(0, public_input.size() - 1, false, var::column_type::public_input);
                        public_input.push_back(original_proof.commitments.lookup.sorted[i].unshifted[j].Y);
                        circuit_proof.comm.lookup_sorted[i].parts[j].Y =
                            var(0, public_input.size() - 1, false, var::column_type::public_input);
                    }
                }

                for (std::size_t j = 0; j < original_proof.commitments.lookup.aggreg.unshifted.size(); j++) {
                    assert(circuit_proof.comm.lookup_agg.parts.size() > j);
                    public_input.push_back(original_proof.commitments.lookup.aggreg.unshifted[j].X);
                    circuit_proof.comm.lookup_agg.parts[j].X =
                        var(0, public_input.size() - 1, false, var::column_type::public_input);
                    public_input.push_back(original_proof.commitments.lookup.aggreg.unshifted[j].Y);
                    circuit_proof.comm.lookup_agg.parts[j].Y =
                        var(0, public_input.size() - 1, false, var::column_type::public_input);
                }

                for (std::size_t j = 0; j < original_proof.commitments.lookup.runtime.unshifted.size(); j++) {
                    assert(circuit_proof.comm.lookup_runtime.parts.size() > j);
                    public_input.push_back(original_proof.commitments.lookup.runtime.unshifted[j].X);
                    circuit_proof.comm.lookup_runtime.parts[j].X =
                        var(0, public_input.size() - 1, false, var::column_type::public_input);
                    public_input.push_back(original_proof.commitments.lookup.runtime.unshifted[j].Y);
                    circuit_proof.comm.lookup_runtime.parts[j].Y =
                        var(0, public_input.size() - 1, false, var::column_type::public_input);
                }

                for (std::size_t j = 0; j < circuit_proof.comm.table.parts.size(); j++) {
                    assert(circuit_proof.comm.table.parts.size() > j);
                    typename CurveType::template g1_type<algebra::curves::coordinates::affine>::value_type point =
                        original_proof.commitments.z_comm.unshifted[0];
                    public_input.push_back(point.X);
                    circuit_proof.comm.table.parts[j].X = var(0, public_input.size() - 1, false, var::column_type::public_input);
                    public_input.push_back(point.Y);
                    circuit_proof.comm.table.parts[j].Y = var(0, public_input.size() - 1, false, var::column_type::public_input);
                }

                // OPENING PROOF
                std::size_t min_lr_size = std::min(original_proof.proof.lr.size(), circuit_proof.o.L.size());
                for (std::size_t i = 0; i < min_lr_size; i++) {
                    public_input.push_back(std::get<0>(original_proof.proof.lr[i]).X);
                    circuit_proof.o.L[i].X = var(0, public_input.size() - 1, false, var::column_type::public_input);
                    public_input.push_back(std::get<0>(original_proof.proof.lr[i]).Y);
                    circuit_proof.o.L[i].Y = var(0, public_input.size() - 1, false, var::column_type::public_input);

                    public_input.push_back(std::get<1>(original_proof.proof.lr[i]).X);
                    circuit_proof.o.R[i].X = var(0, public_input.size() - 1, false, var::column_type::public_input);
                    public_input.push_back(std::get<1>(original_proof.proof.lr[i]).Y);
                    circuit_proof.o.R[i].Y = var(0, public_input.size() - 1, false, var::column_type::public_input);
                }

                public_input.push_back(original_proof.proof.delta.X);
                circuit_proof.o.delta.X = var(0, public_input.size() - 1, false, var::column_type::public_input);
                public_input.push_back(original_proof.proof.delta.Y);
                circuit_proof.o.delta.Y = var(0, public_input.size() - 1, false, var::column_type::public_input);

                public_input.push_back(original_proof.proof.sg.X);
                circuit_proof.o.G.X = var(0, public_input.size() - 1, false, var::column_type::public_input);
                public_input.push_back(original_proof.proof.sg.Y);
                circuit_proof.o.G.Y = var(0, public_input.size() - 1, false, var::column_type::public_input);

                for (std::size_t i = 0; i < kimchi_constants::f_comm_msm_size; i++) {
                    typename BlueprintFieldType::value_type x = 3;
                    public_input.push_back(x);
                    circuit_proof.scalars[i] = var(0, public_input.size() - 1, false, var::column_type::public_input);
                }
            }

            template<typename CurveType, typename BlueprintFieldType, typename KimchiParamsType>
            void prepare_index_base(pallas_verifier_index_type &original_index,
                                    zk::components::kimchi_verifier_index_base<CurveType, KimchiParamsType> &circuit_index,
                                    std::vector<typename BlueprintFieldType::value_type> &public_input) {
                using var = zk::snark::plonk_variable<BlueprintFieldType>;

                // COMMITMENTS
                for (std::size_t i = 0; i < original_index.sigma_comm.size(); i++) {
                    assert(circuit_index.comm.sigma.size() > i);
                    for (std::size_t j = 0; j < original_index.sigma_comm[i].unshifted.size(); j++) {
                        assert(circuit_index.comm.sigma[i].parts.size() > j);
                        public_input.push_back(original_index.sigma_comm[i].unshifted[j].X);
                        circuit_index.comm.sigma[i].parts[j].X =
                            var(0, public_input.size() - 1, false, var::column_type::public_input);
                        public_input.push_back(original_index.sigma_comm[i].unshifted[j].Y);
                        circuit_index.comm.sigma[i].parts[j].Y =
                            var(0, public_input.size() - 1, false, var::column_type::public_input);
                    }
                }

                for (std::size_t i = 0; i < original_index.coefficients_comm.size(); i++) {
                    assert(circuit_index.comm.coefficient.size() > i);
                    for (std::size_t j = 0; j < original_index.coefficients_comm[i].unshifted.size(); j++) {
                        assert(circuit_index.comm.coefficient[i].parts.size() > j);
                        public_input.push_back(original_index.coefficients_comm[i].unshifted[j].X);
                        circuit_index.comm.coefficient[i].parts[j].X =
                            var(0, public_input.size() - 1, false, var::column_type::public_input);
                        public_input.push_back(original_index.coefficients_comm[i].unshifted[j].Y);
                        circuit_index.comm.coefficient[i].parts[j].Y =
                            var(0, public_input.size() - 1, false, var::column_type::public_input);
                    }
                }

                for (std::size_t j = 0; j < original_index.generic_comm.unshifted.size(); j++) {
                    assert(circuit_index.comm.generic.parts.size() > j);
                    public_input.push_back(original_index.generic_comm.unshifted[j].X);
                    circuit_index.comm.generic.parts[j].X = var(0, public_input.size() - 1, false, var::column_type::public_input);
                    public_input.push_back(original_index.generic_comm.unshifted[j].Y);
                    circuit_index.comm.generic.parts[j].Y = var(0, public_input.size() - 1, false, var::column_type::public_input);
                }

                for (std::size_t j = 0; j < original_index.psm_comm.unshifted.size(); j++) {
                    assert(circuit_index.comm.psm.parts.size() > j);
                    public_input.push_back(original_index.psm_comm.unshifted[j].X);
                    circuit_index.comm.psm.parts[j].X = var(0, public_input.size() - 1, false, var::column_type::public_input);
                    public_input.push_back(original_index.psm_comm.unshifted[j].Y);
                    circuit_index.comm.psm.parts[j].Y = var(0, public_input.size() - 1, false, var::column_type::public_input);
                }

                for (std::size_t j = 0; j < original_index.complete_add_comm.unshifted.size(); j++) {
                    assert(circuit_index.comm.complete_add.parts.size() > j);
                    public_input.push_back(original_index.complete_add_comm.unshifted[j].X);
                    circuit_index.comm.complete_add.parts[j].X =
                        var(0, public_input.size() - 1, false, var::column_type::public_input);
                    public_input.push_back(original_index.complete_add_comm.unshifted[j].Y);
                    circuit_index.comm.complete_add.parts[j].Y =
                        var(0, public_input.size() - 1, false, var::column_type::public_input);
                }

                for (std::size_t j = 0; j < original_index.mul_comm.unshifted.size(); j++) {
                    assert(circuit_index.comm.var_base_mul.parts.size() > j);
                    public_input.push_back(original_index.mul_comm.unshifted[j].X);
                    circuit_index.comm.var_base_mul.parts[j].X =
                        var(0, public_input.size() - 1, false, var::column_type::public_input);
                    public_input.push_back(original_index.mul_comm.unshifted[j].Y);
                    circuit_index.comm.var_base_mul.parts[j].Y =
                        var(0, public_input.size() - 1, false, var::column_type::public_input);
                }

                for (std::size_t j = 0; j < original_index.emul_comm.unshifted.size(); j++) {
                    assert(circuit_index.comm.endo_mul.parts.size() > j);
                    public_input.push_back(original_index.emul_comm.unshifted[j].X);
                    circuit_index.comm.endo_mul.parts[j].X = var(0, public_input.size() - 1, false, var::column_type::public_input);
                    public_input.push_back(original_index.emul_comm.unshifted[j].Y);
                    circuit_index.comm.endo_mul.parts[j].Y = var(0, public_input.size() - 1, false, var::column_type::public_input);
                }

                for (std::size_t j = 0; j < original_index.endomul_scalar_comm.unshifted.size(); j++) {
                    assert(circuit_index.comm.endo_mul_scalar.parts.size() > j);
                    public_input.push_back(original_index.endomul_scalar_comm.unshifted[j].X);
                    circuit_index.comm.endo_mul_scalar.parts[j].X =
                        var(0, public_input.size() - 1, false, var::column_type::public_input);
                    public_input.push_back(original_index.endomul_scalar_comm.unshifted[j].Y);
                    circuit_index.comm.endo_mul_scalar.parts[j].Y =
                        var(0, public_input.size() - 1, false, var::column_type::public_input);
                }

                for (std::size_t i = 0; i < original_index.chacha_comm.size(); i++) {
                    assert(circuit_index.comm.chacha.size() > i);
                    for (std::size_t j = 0; j < original_index.chacha_comm[i].unshifted.size(); j++) {
                        assert(circuit_index.comm.chacha[i].parts.size() > j);
                        public_input.push_back(original_index.chacha_comm[i].unshifted[j].X);
                        circuit_index.comm.chacha[i].parts[j].X =
                            var(0, public_input.size() - 1, false, var::column_type::public_input);
                        public_input.push_back(original_index.chacha_comm[i].unshifted[j].Y);
                        circuit_index.comm.chacha[i].parts[j].Y =
                            var(0, public_input.size() - 1, false, var::column_type::public_input);
                    }
                }

                for (std::size_t i = 0; i < original_index.range_check_comm.size(); i++) {
                    assert(circuit_index.comm.range_check.size() > i);
                    for (std::size_t j = 0; j < original_index.range_check_comm[i].unshifted.size(); j++) {
                        assert(circuit_index.comm.range_check[i].parts.size() > j);
                        public_input.push_back(original_index.range_check_comm[i].unshifted[j].X);
                        circuit_index.comm.range_check[i].parts[j].X =
                            var(0, public_input.size() - 1, false, var::column_type::public_input);
                        public_input.push_back(original_index.range_check_comm[i].unshifted[j].Y);
                        circuit_index.comm.range_check[i].parts[j].Y =
                            var(0, public_input.size() - 1, false, var::column_type::public_input);
                    }
                }

                typename CurveType::template g1_type<algebra::curves::coordinates::affine>::value_type point =
                    original_index.sigma_comm[0].unshifted[0];

                for (std::size_t i = 0; i < circuit_index.comm.selectors.size(); i++) {
                    for (std::size_t j = 0; j < circuit_index.comm.selectors[i].parts.size(); j++) {
                        public_input.push_back(point.X);
                        circuit_index.comm.selectors[i].parts[j].X =
                            var(0, public_input.size() - 1, false, var::column_type::public_input);
                        public_input.push_back(point.Y);
                        circuit_index.comm.selectors[i].parts[j].Y =
                            var(0, public_input.size() - 1, false, var::column_type::public_input);
                    }
                }

                for (std::size_t i = 0; i < circuit_index.comm.lookup_selectors.size(); i++) {
                    for (std::size_t j = 0; j < circuit_index.comm.lookup_selectors[i].parts.size(); j++) {
                        public_input.push_back(point.X);
                        circuit_index.comm.lookup_selectors[i].parts[j].X =
                            var(0, public_input.size() - 1, false, var::column_type::public_input);
                        public_input.push_back(point.Y);
                        circuit_index.comm.lookup_selectors[i].parts[j].Y =
                            var(0, public_input.size() - 1, false, var::column_type::public_input);
                    }
                }

                for (std::size_t i = 0; i < circuit_index.comm.lookup_table.size(); i++) {
                    for (std::size_t j = 0; j < circuit_index.comm.lookup_table[i].parts.size(); j++) {
                        public_input.push_back(point.X);
                        circuit_index.comm.lookup_table[i].parts[j].X =
                            var(0, public_input.size() - 1, false, var::column_type::public_input);
                        public_input.push_back(point.Y);
                        circuit_index.comm.lookup_table[i].parts[j].Y =
                            var(0, public_input.size() - 1, false, var::column_type::public_input);
                    }
                }

                for (std::size_t j = 0; j < circuit_index.comm.runtime_tables_selector.parts.size(); j++) {
                    public_input.push_back(point.X);
                    circuit_index.comm.runtime_tables_selector.parts[j].X =
                        var(0, public_input.size() - 1, false, var::column_type::public_input);
                    public_input.push_back(point.Y);
                    circuit_index.comm.runtime_tables_selector.parts[j].Y =
                        var(0, public_input.size() - 1, false, var::column_type::public_input);
                }

                // POINTS
                public_input.push_back(point.X);    // todo srs.h is not used during parsing
                circuit_index.H.X = var(0, public_input.size() - 1, false, var::column_type::public_input);
                public_input.push_back(point.Y);
                circuit_index.H.Y = var(0, public_input.size() - 1, false, var::column_type::public_input);

                for (std::size_t i = 0; i < KimchiParamsType::commitment_params_type::srs_len; i++) {
                    public_input.push_back(point.X);
                    circuit_index.G[i].X = var(0, public_input.size() - 1, false, var::column_type::public_input);
                    public_input.push_back(point.Y);
                    circuit_index.G[i].Y = var(0, public_input.size() - 1, false, var::column_type::public_input);
                }

                for (std::size_t i = 0; i < KimchiParamsType::public_input_size; i++) {
                    public_input.push_back(point.X);    // todo srs.lagrange_bases is not used during parsing
                    circuit_index.lagrange_bases[i].X = var(0, public_input.size() - 1, false, var::column_type::public_input);
                    public_input.push_back(point.Y);
                    circuit_index.lagrange_bases[i].Y = var(0, public_input.size() - 1, false, var::column_type::public_input);
                }
            }

            template<typename CurveType, typename BlueprintFieldType, typename KimchiParamsType>
            void prepare_index_scalar(pallas_verifier_index_type &original_index,
                                    zk::components::kimchi_verifier_index_scalar<BlueprintFieldType> &circuit_index,
                                    std::vector<typename BlueprintFieldType::value_type> &public_input) {
                using var = zk::snark::plonk_variable<BlueprintFieldType>;

                circuit_index.domain_size = original_index.domain.m;

                for (std::size_t i = 0; i < original_index.shift.size(); i++) {
                    public_input.push_back(original_index.shift[i]);
                    circuit_index.shift[i] = var(0, public_input.size() - 1, false, var::column_type::public_input);
                }

                public_input.push_back(original_index.w);
                circuit_index.omega = var(0, public_input.size() - 1, false, var::column_type::public_input);
            }

            template<typename ComponentType, typename BlueprintFieldType, typename ArithmetizationParams, typename Hash,
                    std::size_t Lambda, typename FunctorResultCheck, typename PublicInput,
                    typename std::enable_if<
                        std::is_same<typename BlueprintFieldType::value_type,
                                    typename std::iterator_traits<typename PublicInput::iterator>::value_type>::value,
                        bool>::type = true>
            auto prepare_component(typename ComponentType::params_type params, const PublicInput &public_input,
                                const std::size_t max_step, const FunctorResultCheck &result_check) {

                using ArithmetizationType = zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>;
                using component_type = ComponentType;

                zk::snark::plonk_table_description<BlueprintFieldType, ArithmetizationParams> desc;

                zk::blueprint<ArithmetizationType> bp(desc);
                zk::blueprint_private_assignment_table<ArithmetizationType> private_assignment(desc);
                zk::blueprint_public_assignment_table<ArithmetizationType> public_assignment(desc);
                zk::blueprint_assignment_table<ArithmetizationType> assignment_bp(private_assignment, public_assignment);

                std::size_t start_row = zk::components::allocate<component_type>(bp);
                if (public_input.size() > component_type::rows_amount) {
                    bp.allocate_rows(public_input.size() - component_type::rows_amount);
                }

                for (std::size_t i = 0; i < public_input.size(); i++) {
                    auto allocated_pi = assignment_bp.allocate_public_input(public_input[i]);
                }

                zk::components::generate_circuit<component_type>(bp, public_assignment, params, start_row);
                typename component_type::result_type component_result =
                    component_type::generate_assignments(assignment_bp, params, start_row);

                result_check(assignment_bp, component_result);

                assignment_bp.padding();
                std::cout << "Usable rows: " << desc.usable_rows_amount << std::endl;
                std::cout << "Padded rows: " << desc.rows_amount << std::endl;

                zk::snark::plonk_assignment_table<BlueprintFieldType, ArithmetizationParams> assignments(private_assignment,
                                                                                                        public_assignment);

                using placeholder_params =
                    zk::snark::placeholder_params<BlueprintFieldType, ArithmetizationParams, Hash, Hash, Lambda>;
                using types = zk::snark::detail::placeholder_policy<BlueprintFieldType, placeholder_params>;

                using fri_type = typename zk::commitments::fri<BlueprintFieldType, typename placeholder_params::merkle_hash_type,
                                                            typename placeholder_params::transcript_hash_type, 2, 1>;

                std::size_t table_rows_log = std::ceil(std::log2(desc.rows_amount));

                typename fri_type::params_type fri_params =
                    create_fri_params<fri_type, BlueprintFieldType>(table_rows_log, max_step);

                std::size_t permutation_size = desc.witness_columns + desc.public_input_columns + desc.constant_columns;

                typename zk::snark::placeholder_public_preprocessor<BlueprintFieldType, placeholder_params>::preprocessed_data_type
                    public_preprocessed_data =
                        zk::snark::placeholder_public_preprocessor<BlueprintFieldType, placeholder_params>::process(
                            bp, public_assignment, desc, fri_params, permutation_size);
                typename zk::snark::placeholder_private_preprocessor<BlueprintFieldType, placeholder_params>::preprocessed_data_type
                    private_preprocessed_data =
                        zk::snark::placeholder_private_preprocessor<BlueprintFieldType, placeholder_params>::process(
                            bp, private_assignment, desc, fri_params);

                return std::make_tuple(desc, bp, fri_params, assignments, public_preprocessed_data, private_preprocessed_data);
            }
            #ifdef __EMSCRIPTEN__
            extern "C" {
            template<std::size_t EvalRounds>
            const char *generate_proof_base(zk::snark::pickles_proof<nil::crypto3::algebra::curves::pallas> &pickles_proof,
                                            pallas_verifier_index_type &pickles_index, const std::size_t fri_max_step,
                                            std::string output_path) {
            #else
            template<std::size_t EvalRounds>
            std::string generate_proof_base(zk::snark::proof_type<nil::crypto3::algebra::curves::pallas> &pickles_proof,
                                            pallas_verifier_index_type &pickles_index, const std::size_t fri_max_step,
                                            std::string output_path) {
            #endif
                using curve_type = algebra::curves::pallas;
                using BlueprintFieldType = typename curve_type::base_field_type;
                constexpr std::size_t WitnessColumns = 15;
                constexpr std::size_t PublicInputColumns = 1;
                constexpr std::size_t ConstantColumns = 1;
                constexpr std::size_t SelectorColumns = 30;
                using ArithmetizationParams =
                    zk::snark::plonk_arithmetization_params<WitnessColumns, PublicInputColumns, ConstantColumns, SelectorColumns>;
                using ArithmetizationType = zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>;
                using AssignmentType = zk::blueprint_assignment_table<ArithmetizationType>;
                using hash_type = nil::crypto3::hashes::keccak_1600<256>;
                constexpr std::size_t Lambda = 1;

                using var = zk::snark::plonk_variable<BlueprintFieldType>;

                constexpr static std::size_t public_input_size = 0;
                constexpr static std::size_t max_poly_size = 1 << EvalRounds;    // 32768 in json
                constexpr static std::size_t srs_len = max_poly_size;
                constexpr static std::size_t eval_rounds = EvalRounds;    // 15 in json

                constexpr static std::size_t witness_columns = 15;
                constexpr static std::size_t perm_size = 7;
                constexpr static std::size_t lookup_table_size = 0;
                constexpr static bool use_lookup = false;

                constexpr static std::size_t batch_size = 1;

                constexpr static const std::size_t prev_chal_size = 0;

                using commitment_params = zk::components::kimchi_commitment_params_type<eval_rounds, max_poly_size, srs_len>;
                using index_terms_list = zk::components::index_terms_scalars_list_ec_test<ArithmetizationType>;
                using circuit_description =
                    zk::components::kimchi_circuit_description<index_terms_list, witness_columns, perm_size>;
                using kimchi_params = zk::components::kimchi_params_type<curve_type, commitment_params, circuit_description,
                                                                        public_input_size, prev_chal_size>;

                using component_type = zk::components::base_field<ArithmetizationType, curve_type, kimchi_params, commitment_params,
                                                                batch_size, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14>;

                using fq_output_type =
                    typename zk::components::binding<ArithmetizationType, BlueprintFieldType, kimchi_params>::fq_sponge_output;

                using fr_data_type = typename zk::components::binding<ArithmetizationType, BlueprintFieldType,
                                                                    kimchi_params>::template fr_data<var, batch_size>;

                using fq_data_type =
                    typename zk::components::binding<ArithmetizationType, BlueprintFieldType, kimchi_params>::template fq_data<var>;

                std::vector<typename BlueprintFieldType::value_type> public_input = {};

                std::array<zk::components::kimchi_proof_base<BlueprintFieldType, kimchi_params>, batch_size> proofs;

                for (std::size_t batch_id = 0; batch_id < batch_size; batch_id++) {
                    zk::components::kimchi_proof_base<BlueprintFieldType, kimchi_params> proof;

                    prepare_proof_base<curve_type, BlueprintFieldType, kimchi_params, eval_rounds>(pickles_proof, proof,
                                                                                                public_input);

                    proofs[batch_id] = proof;
                }

                zk::components::kimchi_verifier_index_base<curve_type, kimchi_params> verifier_index;
                prepare_index_base<curve_type, BlueprintFieldType, kimchi_params>(pickles_index, verifier_index, public_input);

                fr_data_type fr_data_public;
                fq_data_type fq_data_public;

                public_input.push_back(3);
                for (std::size_t i = 0; i < fr_data_public.scalars.size(); i++) {
                    var s(0, public_input.size() - 1, false, var::column_type::public_input);
                    fr_data_public.scalars[i] = s;
                }

                typename component_type::params_type params = {proofs, verifier_index, fr_data_public, fq_data_public};

                auto result_check = [](AssignmentType &assignment, typename component_type::result_type &real_res) {};

                // using Endianness = nil::marshalling::option::big_endian;

                using placeholder_params =
                    zk::snark::placeholder_params<BlueprintFieldType, ArithmetizationParams, hash_type, hash_type, Lambda>;

                auto [desc, bp, fri_params, assignments, public_preprocessed_data, private_preprocessed_data] =
                    prepare_component<component_type, BlueprintFieldType, ArithmetizationParams, hash_type, Lambda>(
                        params, public_input, fri_max_step, result_check);

                auto proof = zk::snark::placeholder_prover<BlueprintFieldType, placeholder_params>::process(
                    public_preprocessed_data, private_preprocessed_data, desc, bp, assignments, fri_params);

                bool verifier_res = zk::snark::placeholder_verifier<BlueprintFieldType, placeholder_params>::process(
                    public_preprocessed_data, proof, bp, fri_params);

                if (verifier_res) {
                    std::cout << "Inner verification passed" << std::endl;
                } else {
                    std::cout << "Inner verification failed" << std::endl;
                }

                std::string output_path_full = output_path + "_base";
                proof_print<nil::marshalling::option::big_endian>(proof, output_path_full);

                // std::string st = marshalling_to_blob<Endianness>(proof);
                std::string st;
            #ifdef __EMSCRIPTEN__
                char *writable = new char[st.size() + 1];
                std::copy(st.begin(), st.end(), writable);
                return writable;
            #else
                return st;
            #endif
            }

            #ifdef __EMSCRIPTEN__
            extern "C" {
            template<std::size_t EvalRounds>
            const char *generate_proof_scalar(zk::snark::pickles_proof<nil::crypto3::algebra::curves::pallas> &pickles_proof,
                                            pallas_verifier_index_type &pickles_index, const std::size_t fri_max_step,
                                            std::string output_path) {
            #else
            template<std::size_t EvalRounds>
            std::string generate_proof_scalar(zk::snark::proof_type<nil::crypto3::algebra::curves::pallas> &pickles_proof,
                                            pallas_verifier_index_type &pickles_index, const std::size_t fri_max_step,
                                            std::string output_path) {
            #endif
                using curve_type = algebra::curves::pallas;
                using BlueprintFieldType = typename curve_type::scalar_field_type;
                constexpr std::size_t WitnessColumns = 15;
                constexpr std::size_t PublicInputColumns = 1;
                constexpr std::size_t ConstantColumns = 1;
                constexpr std::size_t SelectorColumns = 30;
                using ArithmetizationParams =
                    zk::snark::plonk_arithmetization_params<WitnessColumns, PublicInputColumns, ConstantColumns, SelectorColumns>;
                using ArithmetizationType = zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>;
                using AssignmentType = zk::blueprint_assignment_table<ArithmetizationType>;
                using hash_type = nil::crypto3::hashes::keccak_1600<256>;
                constexpr std::size_t Lambda = 1;

                using var = zk::snark::plonk_variable<BlueprintFieldType>;

                constexpr static std::size_t public_input_size = 1;
                constexpr static std::size_t max_poly_size = 1 << EvalRounds;    // 32768 in json
                constexpr static std::size_t srs_len = max_poly_size;
                constexpr static std::size_t eval_rounds = EvalRounds;    // 15 in json

                constexpr static std::size_t witness_columns = 15;
                constexpr static std::size_t perm_size = 7;
                constexpr static std::size_t lookup_table_size = 0;

                constexpr static std::size_t batch_size = 1;

                constexpr static const std::size_t prev_chal_size = 0;

                using commitment_params = zk::components::kimchi_commitment_params_type<eval_rounds, max_poly_size, srs_len>;
                using index_terms_list = zk::components::index_terms_scalars_list_ec_test<ArithmetizationType>;
                using circuit_description =
                    zk::components::kimchi_circuit_description<index_terms_list, witness_columns, perm_size>;
                using kimchi_params = zk::components::kimchi_params_type<curve_type, commitment_params, circuit_description,
                                                                        public_input_size, prev_chal_size>;

                using component_type =
                    zk::components::verify_scalar<ArithmetizationType, curve_type, kimchi_params, commitment_params, batch_size, 0,
                                                1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14>;

                using fq_output_type =
                    typename zk::components::binding<ArithmetizationType, BlueprintFieldType, kimchi_params>::fq_sponge_output;

                using fr_data_type = typename zk::components::binding<ArithmetizationType, BlueprintFieldType,
                                                                    kimchi_params>::template fr_data<var, batch_size>;

                using fq_data_type =
                    typename zk::components::binding<ArithmetizationType, BlueprintFieldType, kimchi_params>::template fq_data<var>;

                std::vector<typename BlueprintFieldType::value_type> public_input = {0};

                std::array<zk::components::kimchi_proof_scalar<BlueprintFieldType, kimchi_params, eval_rounds>, batch_size> proofs;

                for (std::size_t batch_id = 0; batch_id < batch_size; batch_id++) {
                    zk::components::kimchi_proof_scalar<BlueprintFieldType, kimchi_params, eval_rounds> proof;

                    prepare_proof_scalar<curve_type, BlueprintFieldType, kimchi_params, eval_rounds>(pickles_proof, proof,
                                                                                                    public_input);

                    proofs[batch_id] = proof;
                }

                zk::components::kimchi_verifier_index_scalar<BlueprintFieldType> verifier_index;
                prepare_index_scalar<curve_type, BlueprintFieldType, kimchi_params>(pickles_index, verifier_index, public_input);
                verifier_index.domain_size = max_poly_size;

                using fq_output_type =
                    typename zk::components::binding<ArithmetizationType, BlueprintFieldType, kimchi_params>::fq_sponge_output;

                fr_data_type fr_data_public;
                fq_data_type fq_data_public;
                std::array<fq_output_type, batch_size> fq_outputs;

                typename component_type::params_type params = {fr_data_public, fq_data_public, verifier_index, proofs, fq_outputs};

                auto result_check = [](AssignmentType &assignment, typename component_type::result_type &real_res) {};

                // using Endianness = nil::marshalling::option::big_endian;

                using placeholder_params =
                    zk::snark::placeholder_params<BlueprintFieldType, ArithmetizationParams, hash_type, hash_type, Lambda>;

                auto [desc, bp, fri_params, assignments, public_preprocessed_data, private_preprocessed_data] =
                    prepare_component<component_type, BlueprintFieldType, ArithmetizationParams, hash_type, Lambda>(
                        params, public_input, fri_max_step, result_check);

                auto proof = zk::snark::placeholder_prover<BlueprintFieldType, placeholder_params>::process(
                    public_preprocessed_data, private_preprocessed_data, desc, bp, assignments, fri_params);

                bool verifier_res = zk::snark::placeholder_verifier<BlueprintFieldType, placeholder_params>::process(
                    public_preprocessed_data, proof, bp, fri_params);

                if (verifier_res) {
                    std::cout << "Inner verification passed" << std::endl;
                } else {
                    std::cout << "Inner verification failed" << std::endl;
                }

                std::string output_path_full = output_path + "_scalar";
                proof_print<nil::marshalling::option::big_endian>(proof, output_path_full);

                // std::string st = marshalling_to_blob<Endianness>(proof);
                std::string st;
            #ifdef __EMSCRIPTEN__
                char *writable = new char[st.size() + 1];
                std::copy(st.begin(), st.end(), writable);
                return writable;
            #else
                return st;
            #endif
            }

            void concatenate_proofs(std::string output_path) {
                std::string output_path_scalar = output_path + "_scalar";
                std::string output_path_base = output_path + "_base";
                std::string output_path_full = output_path + "_full";
                std::ifstream file_scalar(output_path_scalar, std::ios::binary);
                std::ifstream file_base(output_path_base, std::ios::binary);
                std::ofstream file_full(output_path_full, std::ios::binary);

                file_scalar.ignore(2);

                file_full << file_base.rdbuf();
                file_full << file_scalar.rdbuf();
            }

            template<std::size_t EvalRoundsScalar, std::size_t EvalRoundsBase>
            void generate_proof_heterogenous(zk::snark::proof_type<nil::crypto3::algebra::curves::pallas> &pickles_proof,
                                            pallas_verifier_index_type &pickles_index, const std::size_t fri_max_step,
                                            std::string output_path) {

                generate_proof_scalar<EvalRoundsScalar>(pickles_proof, pickles_index, fri_max_step, output_path);
                generate_proof_base<EvalRoundsBase>(pickles_proof, pickles_index, fri_max_step, output_path);

                concatenate_proofs(output_path);
            }

            void proof_new(boost::json::value jv, boost::json::value jv_public_input, std::string output_file) {
                zk::snark::proof_type<nil::crypto3::algebra::curves::pallas> proof = make_proof(jv_public_input);
                pallas_verifier_index_type ver_index = make_verify_index(jv_public_input, jv);

                constexpr const std::size_t eval_rounds_scalar = 15;
                constexpr const std::size_t eval_rounds_base = 10;
                constexpr const std::size_t fri_max_step = 1;

                generate_proof_heterogenous<eval_rounds_scalar, eval_rounds_base>(proof, ver_index, fri_max_step, output_file);
            }
        }
    }
}