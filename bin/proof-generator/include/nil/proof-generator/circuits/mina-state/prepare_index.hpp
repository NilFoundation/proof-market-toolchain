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

#ifndef PROOF_GENERATOR_CIRCUITS_MINA_STATE_PREPARE_INDEX_HPP
#define PROOF_GENERATOR_CIRCUITS_MINA_STATE_PREPARE_INDEX_HPP

#include <nil/crypto3/algebra/curves/pallas.hpp>

namespace nil {
    namespace proof_generator {
        namespace mina_state {

            template<typename VerifierIndexType,
                     typename CurveType,
                     typename BlueprintFieldType,
                     typename KimchiParamsType>
            void prepare_index_base(
                VerifierIndexType &original_index,
                nil::blueprint_mc::components::kimchi_verifier_index_base<CurveType, KimchiParamsType> &circuit_index,
                std::vector<typename BlueprintFieldType::value_type> &public_input) {
                using var = nil::crypto3::zk::snark::plonk_variable<BlueprintFieldType>;

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
                    circuit_index.comm.generic.parts[j].X =
                        var(0, public_input.size() - 1, false, var::column_type::public_input);
                    public_input.push_back(original_index.generic_comm.unshifted[j].Y);
                    circuit_index.comm.generic.parts[j].Y =
                        var(0, public_input.size() - 1, false, var::column_type::public_input);
                }

                for (std::size_t j = 0; j < original_index.psm_comm.unshifted.size(); j++) {
                    assert(circuit_index.comm.psm.parts.size() > j);
                    public_input.push_back(original_index.psm_comm.unshifted[j].X);
                    circuit_index.comm.psm.parts[j].X =
                        var(0, public_input.size() - 1, false, var::column_type::public_input);
                    public_input.push_back(original_index.psm_comm.unshifted[j].Y);
                    circuit_index.comm.psm.parts[j].Y =
                        var(0, public_input.size() - 1, false, var::column_type::public_input);
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
                    circuit_index.comm.endo_mul.parts[j].X =
                        var(0, public_input.size() - 1, false, var::column_type::public_input);
                    public_input.push_back(original_index.emul_comm.unshifted[j].Y);
                    circuit_index.comm.endo_mul.parts[j].Y =
                        var(0, public_input.size() - 1, false, var::column_type::public_input);
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

                typename CurveType::template g1_type<nil::crypto3::algebra::curves::coordinates::affine>::value_type
                    point = original_index.sigma_comm[0].unshifted[0];

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
                    circuit_index.lagrange_bases[i].X =
                        var(0, public_input.size() - 1, false, var::column_type::public_input);
                    public_input.push_back(point.Y);
                    circuit_index.lagrange_bases[i].Y =
                        var(0, public_input.size() - 1, false, var::column_type::public_input);
                }
            }

            template<typename VerifierIndexType,
                     typename CurveType,
                     typename BlueprintFieldType,
                     typename KimchiParamsType>
            void prepare_index_scalar(
                VerifierIndexType &original_index,
                nil::blueprint_mc::components::kimchi_verifier_index_scalar<BlueprintFieldType> &circuit_index,
                std::vector<typename BlueprintFieldType::value_type> &public_input) {
                using var = nil::crypto3::zk::snark::plonk_variable<BlueprintFieldType>;

                circuit_index.domain_size = original_index.domain.m;

                for (std::size_t i = 0; i < original_index.shift.size(); i++) {
                    public_input.push_back(original_index.shift[i]);
                    circuit_index.shift[i] = var(0, public_input.size() - 1, false, var::column_type::public_input);
                }

                public_input.push_back(original_index.w);
                circuit_index.omega = var(0, public_input.size() - 1, false, var::column_type::public_input);
            }
        }    // namespace mina_state
    }        // namespace proof_generator
}    // namespace nil

#endif    // PROOF_GENERATOR_CIRCUITS_MINA_STATE_PREPARE_INDEX_HPP