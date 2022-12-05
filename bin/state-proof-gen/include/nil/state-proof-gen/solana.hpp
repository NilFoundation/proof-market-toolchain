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

#include <boost/json/src.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/optional.hpp>

#include <nil/crypto3/pubkey/eddsa.hpp>

#include <nil/crypto3/algebra/curves/ed25519.hpp>
#include <nil/crypto3/algebra/fields/arithmetic_params/ed25519.hpp>


#include <nil/crypto3/hash/keccak.hpp>

#include <nil/marshalling/algorithms/pack.hpp>
#include <nil/marshalling/status_type.hpp>

#include <nil/crypto3/marshalling/multiprecision/types/integral.hpp>


#include <nil/crypto3/zk/components/non_native/algebra/fields/plonk/signatures_verification.hpp>
#include <nil/crypto3/zk/components/non_native/algebra/fields/plonk/ed25519.hpp>

#include <nil/state-proof-gen/proof_generate.hpp>
#include <nil/state-proof-gen/structs_deserialization.hpp>
#include <nil/state-proof-gen/ec_index_terms.hpp>

#include <nil/crypto3/zk/commitments/type_traits.hpp>
#include <nil/crypto3/zk/snark/arithmetization/plonk/params.hpp>
#include <nil/crypto3/zk/snark/systems/plonk/placeholder/preprocessor.hpp>
#include <nil/crypto3/zk/snark/systems/plonk/placeholder/prover.hpp>
#include <nil/crypto3/zk/snark/systems/plonk/placeholder/verifier.hpp>
#include <nil/crypto3/zk/snark/systems/plonk/placeholder/params.hpp>


namespace nil {
    namespace crypto3 {
        namespace solana_state_proof {

            template<typename Hash, typename CurveType>
            struct signature_t {
                typedef typename CurveType::base_field_type::integral_type integral_type;
                typedef typename CurveType::scalar_field_type::value_type value_type;
                std::array<integral_type, 2> points;
                value_type scalar;
            };

            template<typename Hash, typename CurveType>
            struct vote_state {
                typedef Hash hash_type;
                typedef typename Hash::digest_type digest_type;
                typedef typename CurveType::base_field_type::integral_type integral_type;
                signature_t<Hash, CurveType> signature;
                std::array<integral_type, 2> pubkey;

                std::size_t weight;
            };

            template<typename Hash, typename CurveType>
            struct block_data {
                typedef Hash hash_type;
                typedef typename Hash::digest_type digest_type;

                std::size_t block_number;
                digest_type bank_hash;
                digest_type previous_bank_hash;
                std::uint32_t timestamp;
            };

            template<typename Hash, typename CurveType>
            struct state_type {
                typedef Hash hash_type;
                typedef typename Hash::digest_type digest_type;
                typedef CurveType signature_scheme_type;

                std::size_t confirmed;
                std::size_t new_confirmed;
                digest_type bank_hash;
                std::vector<block_data<hash_type, signature_scheme_type>> repl_data;
                std::vector<vote_state<Hash, signature_scheme_type>> votes;
            };

            template<typename Hash, typename CurveType>
            vote_state<Hash, CurveType>
            tag_invoke(boost::json::value_to_tag<vote_state<Hash, CurveType>>, const boost::json::value &jv) {
                auto &o = jv.as_object();
                return {
                        .signature =
                        [&](const boost::json::value &v) {
                            nil::marshalling::status_type status;
                            typedef typename CurveType::base_field_type::integral_type integral_type;
                            typedef typename CurveType::scalar_field_type::value_type value_type;
                            std::vector<uint8_t> st_decode = nil::crypto3::decode<nil::crypto3::codec::base<58>>(boost::json::value_to<std::string>(v));
                            integral_type x = nil::marshalling::pack<nil::marshalling::option::big_endian>(st_decode.begin(),
                                                                                                        st_decode.begin() +
                                                                                                        st_decode.size() / 3,
                                                                                                        status);
                            std::vector<uint8_t> st_decode2(st_decode.begin() + st_decode.size() / 3,
                                                            st_decode.begin() + st_decode.size() / 3 * 2);
                            integral_type y = nil::marshalling::pack<nil::marshalling::option::big_endian>(st_decode2, status);

                            std::vector<uint8_t> st_decode3(st_decode.begin() + st_decode.size() / 3 * 2, st_decode.end());
                            value_type s = nil::marshalling::pack<nil::marshalling::option::big_endian>(st_decode3, status);
                            return signature_t<Hash, CurveType>{.points = {x, y}, .scalar = s};
                        }(o.at("signature")),
                        .pubkey =
                        [&](const boost::json::value &v) {
                            nil::marshalling::status_type status;
                            typedef typename CurveType::base_field_type::integral_type integral_type;
                            std::vector<uint8_t> st_decode = nil::crypto3::decode<nil::crypto3::codec::base<58>>(boost::json::value_to<std::string>(v));
                            integral_type x = nil::marshalling::pack<nil::marshalling::option::big_endian>(st_decode.begin(),
                                                                                                        st_decode.begin() +
                                                                                                        st_decode.size() / 2,
                                                                                                        status);
                            std::vector<uint8_t> st_decode2(st_decode.begin() + st_decode.size() / 2, st_decode.end());
                            integral_type y = nil::marshalling::pack<nil::marshalling::option::big_endian>(st_decode2, status);

                            return std::array<typename CurveType::base_field_type::integral_type, 2>{x, y};
                        }(o.at("pubkey")),
                        .weight = boost::json::value_to<std::size_t>(o.at("weight"))};
            }

            template<typename Hash, typename CurveType>
            block_data<Hash, CurveType>
            tag_invoke(boost::json::value_to_tag<block_data<Hash, CurveType>>, const boost::json::value &jv) {
                auto &o = jv.as_object();

                return {
                        .block_number = boost::json::value_to<std::size_t>(o.at("block_number")),
                        .bank_hash =
                        [&](const boost::json::value &v) {
                            typename Hash::digest_type ret;
                            nil::crypto3::decode<nil::crypto3::codec::base<58>>(
                                    boost::json::value_to<std::string>(v), ret.begin());

                            return ret;
                        }(o.at("bank_hash")),
                        .previous_bank_hash =
                        [&](const boost::json::value &v) {
                            typename Hash::digest_type ret;
                            nil::crypto3::decode<nil::crypto3::codec::base<58>>(
                                    boost::json::value_to<std::string>(v), ret.begin());
                            return ret;
                        }(o.at("previous_bank_hash")),
                        .timestamp = boost::json::value_to<std::uint32_t>(o.at("timestamp")),
                };
            }

            template<typename Hash, typename CurveType>
            state_type<Hash, CurveType> tag_invoke(boost::json::value_to_tag<state_type<Hash, CurveType>>,
                                                const boost::json::value &jv) {
                auto &o = jv.as_object();
                return {.confirmed = boost::json::value_to<std::size_t>(o.at("confirmed")),
                        .new_confirmed = boost::json::value_to<std::size_t>(o.at("new_confirmed")),
                        .bank_hash =
                        [&](const boost::json::value &v) {
                            typename Hash::digest_type ret;
                            nil::crypto3::decode<nil::crypto3::codec::base<58>>(
                                    boost::json::value_to<std::string>(v), ret.begin());
                            return ret;
                        }(o.at("bank_hash")),
                        .repl_data =
                        [&](const boost::json::value &arr) {
                            std::vector<block_data<Hash, CurveType>> ret;
                            for (const boost::json::value &val: arr.as_array()) {
                                ret.emplace_back(boost::json::value_to<block_data<Hash, CurveType>>(val));
                            }
                            return ret;
                        }(o.at("repl_data")),
                        .votes =
                        [&](const boost::json::value &arr) {
                            std::vector<vote_state<Hash, CurveType>> ret;
                            for (const boost::json::value &val: arr.as_array()) {
                                ret.emplace_back(boost::json::value_to<vote_state<Hash, CurveType>>(val));
                            }
                            return ret;
                        }(o.at("votes"))
                };
            }


            void proof_new(boost::json::value jv, boost::json::value jv_public_input, std::string output_file) {
                using curve_type = algebra::curves::pallas;
                using ed25519_type = algebra::curves::ed25519;
                using BlueprintFieldType = typename curve_type::base_field_type;
                constexpr std::size_t WitnessColumns = 9;
                constexpr std::size_t PublicInputColumns = 1;
                constexpr std::size_t ConstantColumns = 1;
                constexpr std::size_t SelectorColumns = 26;
                using ArithmetizationParams =
                        zk::snark::plonk_arithmetization_params<WitnessColumns, PublicInputColumns, ConstantColumns, SelectorColumns>;
                using ArithmetizationType = zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>;
                using AssignmentType = zk::blueprint_assignment_table<ArithmetizationType>;
                using hash_type = nil::crypto3::hashes::keccak_1600<256>;
                constexpr std::size_t Lambda = 1;

                using var = zk::snark::plonk_variable<BlueprintFieldType>;
                constexpr const std::size_t k = 1;
                using component_type = zk::components::signatures_verification<ArithmetizationType, curve_type, ed25519_type, k, 0, 1, 2, 3,
                        4, 5, 6, 7, 8>;
                using ed25519_component = zk::components::eddsa25519<ArithmetizationType, curve_type, ed25519_type,
                        0, 1, 2, 3, 4, 5, 6, 7, 8>;
                using var_ec_point = typename ed25519_component::params_type::var_ec_point;
                using signature = typename ed25519_component::params_type::signature;


                state_type<hash_type, ed25519_type> state;
                state = boost::json::value_to<state_type<hash_type, ed25519_type>>(jv_public_input.at("state"));

                ed25519_type::template g1_type<algebra::curves::coordinates::affine>::value_type B =
                        ed25519_type::template g1_type<algebra::curves::coordinates::affine>::value_type::one();


                nil::marshalling::status_type status;
                multiprecision::number<multiprecision::cpp_int_backend<256, 256>> M = pack(state.bank_hash.begin(), state.bank_hash.end(), status);
                std::vector<typename BlueprintFieldType::value_type> public_input;
                typename ed25519_type::base_field_type::integral_type base = 1;
                typename ed25519_type::base_field_type::integral_type mask = (base << 66) - 1;
                std::array<signature, k> Signatures;
                std::array<var_ec_point, k> Public_keys;
                std::array<ed25519_type::template g1_type<algebra::curves::coordinates::affine>::value_type, k> Signatures_point;
                std::array<ed25519_type::scalar_field_type::value_type, k> Signatures_scalar;
                std::array<ed25519_type::template g1_type<algebra::curves::coordinates::affine>::value_type, k> Public_keys_values;
                for(std::size_t i = 0; i < k; i++) {

                    ed25519_type::scalar_field_type::value_type s = state.votes[0].signature.scalar;
                    ed25519_type::base_field_type::integral_type Rx = ed25519_type::base_field_type::integral_type(
                            state.votes[0].signature.points[0]);
                    ed25519_type::base_field_type::integral_type Ry = ed25519_type::base_field_type::integral_type(
                            state.votes[0].signature.points[1]);
                    ed25519_type::base_field_type::integral_type Px = ed25519_type::base_field_type::integral_type(
                            state.votes[0].pubkey[0]);
                    ed25519_type::base_field_type::integral_type Py = ed25519_type::base_field_type::integral_type(
                            state.votes[0].pubkey[1]);

                    public_input.insert(public_input.end(), {Rx & mask, (Rx >> 66) & mask, (Rx >> 132) & mask, (Rx >> 198) & mask,
                                                            Ry & mask, (Ry >> 66) & mask, (Ry >> 132) & mask, (Ry >> 198) & mask, typename BlueprintFieldType::integral_type(s.data),
                                                            Px & mask, (Px >> 66) & mask, (Px >> 132) & mask, (Px >> 198) & mask,
                                                            Py & mask, (Py >> 66) & mask, (Py >> 132) & mask, (Py >> 198) & mask});
                    std::array<var, 4> e_R_x = {var(0, i*17 + 0, false, var::column_type::public_input), var(0, i*17 + 1, false, var::column_type::public_input),
                                                var(0, i*17 + 2, false, var::column_type::public_input), var(0, i*17 + 3, false, var::column_type::public_input)};
                    std::array<var, 4> e_R_y = {var(0, i*17 + 4, false, var::column_type::public_input), var(0, i*17 + 5, false, var::column_type::public_input),
                                                var(0, i*17 + 6, false, var::column_type::public_input), var(0, i*17 + 7, false, var::column_type::public_input)};
                    var_ec_point R_i = {e_R_x, e_R_y};
                    var e_s = var(0, i*17 + 8, false, var::column_type::public_input);
                    Signatures[i] = {R_i, e_s};
                    std::array<var, 4> pk_x = {var(0, i*17 + 9, false, var::column_type::public_input), var(0, i*17 + 10, false, var::column_type::public_input),
                                            var(0, i*17 + 11, false, var::column_type::public_input), var(0, i*17 + 12, false, var::column_type::public_input)};
                    std::array<var, 4> pk_y = {var(0, i*17 + 13, false, var::column_type::public_input), var(0, i*17 + 14, false, var::column_type::public_input),
                                            var(0, i*17 + 15, false, var::column_type::public_input), var(0, i*17 + 16, false, var::column_type::public_input)};
                    Public_keys[i] = {pk_x, pk_y};
                }
                public_input.insert(public_input.end(), {ed25519_type::base_field_type::integral_type(M & mask), ed25519_type::base_field_type::integral_type((M >> 66) & mask)
                        , ed25519_type::base_field_type::integral_type((M >> 132) & mask), ed25519_type::base_field_type::integral_type((M >> 198) & mask)});

                std::array<var, 4> M_var = {var(0, k*17, false, var::column_type::public_input), var(0, k*17 + 1, false, var::column_type::public_input),
                                            var(0, k*17 + 2, false, var::column_type::public_input), var(0, k*17 + 3, false, var::column_type::public_input)};


                typename component_type::params_type params = {Signatures, Public_keys, M_var};

                auto result_check = [](AssignmentType &assignment,
                           component_type::result_type &real_res) {
                };

                create_component_proof<component_type, BlueprintFieldType, ArithmetizationParams, hash_type, Lambda>(
                    params, public_input, result_check, output_file);
            }
        }
    }
}