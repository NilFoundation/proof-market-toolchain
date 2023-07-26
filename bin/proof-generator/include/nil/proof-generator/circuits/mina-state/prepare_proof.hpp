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

#ifndef PROOF_GENERATOR_CIRCUITS_MINA_STATE_PREPARE_PROOF_HPP
#define PROOF_GENERATOR_CIRCUITS_MINA_STATE_PREPARE_PROOF_HPP

#include <nil/crypto3/algebra/curves/pallas.hpp>

namespace nil {
    namespace proof_generator {
        namespace mina_state {

            template<typename CurveType, typename BlueprintFieldType, typename KimchiParamsType, std::size_t EvalRounds>
            void prepare_proof_scalar(
                nil::crypto3::zk::snark::proof_type<nil::crypto3::algebra::curves::pallas> &original_proof,
                nil::blueprint_mc::components::kimchi_proof_scalar<BlueprintFieldType, KimchiParamsType, EvalRounds>
                    &circuit_proof,
                std::vector<typename BlueprintFieldType::value_type> &public_input) {
                using var = nil::crypto3::zk::snark::plonk_variable<BlueprintFieldType>;

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
                    circuit_proof.proof_evals[point_idx].z =
                        var(0, public_input.size() - 1, false, var::column_type::public_input);
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

                for (std::size_t i = 0; i < KimchiParamsType::public_input_size; i++) {
                    public_input.push_back(scalar_value);
                    circuit_proof.public_input[i] =
                        var(0, public_input.size() - 1, false, var::column_type::public_input);
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
            void prepare_proof_base(
                nil::crypto3::zk::snark::proof_type<nil::crypto3::algebra::curves::pallas> &original_proof,
                nil::blueprint_mc::components::kimchi_proof_base<BlueprintFieldType, KimchiParamsType> &circuit_proof,
                std::vector<typename BlueprintFieldType::value_type> &public_input) {
                using var = nil::crypto3::zk::snark::plonk_variable<BlueprintFieldType>;
                using kimchi_constants = nil::blueprint_mc::components::kimchi_inner_constants<KimchiParamsType>;

                // COMMITMENTS
                for (std::size_t i = 0; i < original_proof.commitments.w_comm.size(); i++) {
                    assert(circuit_proof.comm.witness.size() > i);
                    for (std::size_t j = 0; j < original_proof.commitments.w_comm[i].unshifted.size(); j++) {
                        if (j >= circuit_proof.comm.witness[i].parts.size()) {
                            break;
                        }
                        public_input.push_back(original_proof.commitments.w_comm[i].unshifted[j].X);
                        circuit_proof.comm.witness[i].parts[j].X =
                            var(0, public_input.size() - 1, false, var::column_type::public_input);
                        public_input.push_back(original_proof.commitments.w_comm[i].unshifted[j].Y);
                        circuit_proof.comm.witness[i].parts[j].Y =
                            var(0, public_input.size() - 1, false, var::column_type::public_input);
                    }
                }

                for (std::size_t j = 0; j < original_proof.commitments.z_comm.unshifted.size(); j++) {
                    if (j >= circuit_proof.comm.z.parts.size()) {
                        break;
                    }
                    public_input.push_back(original_proof.commitments.z_comm.unshifted[j].X);
                    circuit_proof.comm.z.parts[j].X =
                        var(0, public_input.size() - 1, false, var::column_type::public_input);
                    public_input.push_back(original_proof.commitments.z_comm.unshifted[j].Y);
                    circuit_proof.comm.z.parts[j].Y =
                        var(0, public_input.size() - 1, false, var::column_type::public_input);
                }

                for (std::size_t j = 0; j < original_proof.commitments.t_comm.unshifted.size(); j++) {
                    if (j >= circuit_proof.comm.t.parts.size()) {
                        break;
                    }
                    public_input.push_back(original_proof.commitments.t_comm.unshifted[j].X);
                    circuit_proof.comm.t.parts[j].X =
                        var(0, public_input.size() - 1, false, var::column_type::public_input);
                    public_input.push_back(original_proof.commitments.t_comm.unshifted[j].Y);
                    circuit_proof.comm.t.parts[j].Y =
                        var(0, public_input.size() - 1, false, var::column_type::public_input);
                }

                for (std::size_t i = 0; i < circuit_proof.comm.lookup_sorted.size(); i++) {
                    for (std::size_t j = 0; j < original_proof.commitments.lookup.sorted[i].unshifted.size(); j++) {
                        if (j >= circuit_proof.comm.lookup_sorted[i].parts.size()) {
                            break;
                        }
                        public_input.push_back(original_proof.commitments.lookup.sorted[i].unshifted[j].X);
                        circuit_proof.comm.lookup_sorted[i].parts[j].X =
                            var(0, public_input.size() - 1, false, var::column_type::public_input);
                        public_input.push_back(original_proof.commitments.lookup.sorted[i].unshifted[j].Y);
                        circuit_proof.comm.lookup_sorted[i].parts[j].Y =
                            var(0, public_input.size() - 1, false, var::column_type::public_input);
                    }
                }

                for (std::size_t j = 0; j < original_proof.commitments.lookup.aggreg.unshifted.size(); j++) {
                    if (j >= circuit_proof.comm.lookup_agg.parts.size()) {
                        break;
                    }
                    public_input.push_back(original_proof.commitments.lookup.aggreg.unshifted[j].X);
                    circuit_proof.comm.lookup_agg.parts[j].X =
                        var(0, public_input.size() - 1, false, var::column_type::public_input);
                    public_input.push_back(original_proof.commitments.lookup.aggreg.unshifted[j].Y);
                    circuit_proof.comm.lookup_agg.parts[j].Y =
                        var(0, public_input.size() - 1, false, var::column_type::public_input);
                }

                for (std::size_t j = 0; j < original_proof.commitments.lookup.runtime.unshifted.size(); j++) {
                    if (j >= circuit_proof.comm.lookup_runtime.parts.size()) {
                        break;
                    }
                    public_input.push_back(original_proof.commitments.lookup.runtime.unshifted[j].X);
                    circuit_proof.comm.lookup_runtime.parts[j].X =
                        var(0, public_input.size() - 1, false, var::column_type::public_input);
                    public_input.push_back(original_proof.commitments.lookup.runtime.unshifted[j].Y);
                    circuit_proof.comm.lookup_runtime.parts[j].Y =
                        var(0, public_input.size() - 1, false, var::column_type::public_input);
                }

                for (std::size_t j = 0; j < circuit_proof.comm.table.parts.size(); j++) {
                    typename CurveType::template g1_type<nil::crypto3::algebra::curves::coordinates::affine>::value_type
                        point = original_proof.commitments.z_comm.unshifted[0];
                    public_input.push_back(point.X);
                    circuit_proof.comm.table.parts[j].X =
                        var(0, public_input.size() - 1, false, var::column_type::public_input);
                    public_input.push_back(point.Y);
                    circuit_proof.comm.table.parts[j].Y =
                        var(0, public_input.size() - 1, false, var::column_type::public_input);
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
        }    // namespace mina_state
    }        // namespace proof_generator
}    // namespace nil

#endif    // PROOF_GENERATOR_CIRCUITS_MINA_STATE_PREPARE_PROOF_HPP