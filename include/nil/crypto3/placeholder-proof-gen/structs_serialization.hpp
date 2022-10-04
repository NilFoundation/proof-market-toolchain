//---------------------------------------------------------------------------//
// Copyright (c) 2022 Mikhail Komarov <nemo@nil.foundation>
// Copyright (c) 2022 Aleksei Moskvin <alalmoskvin@nil.foundation>
//
// MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//---------------------------------------------------------------------------//

#ifndef PLACEHOLDER_PROOF_GEN_SERIALIZATION_STRUCTS_HPP
#define PLACEHOLDER_PROOF_GEN_SERIALIZATION_STRUCTS_HPP


#include <boost/json/src.hpp>

#include <nil/crypto3/zk/blueprint/plonk.hpp>
#include <nil/crypto3/zk/assignment/plonk.hpp>
#include <nil/crypto3/zk/snark/arithmetization/plonk/table_description.hpp>
#include <nil/crypto3/zk/snark/arithmetization/plonk/params.hpp>

#include <nil/crypto3/container/merkle/tree.hpp>

#include <type_traits>

namespace nil {
    namespace crypto3 {
        namespace zk {
            template<typename ArithmetizationType>
            void tag_invoke(boost::json::value_from_tag, boost::json::value &jv,
                            zk::blueprint_public_assignment_table<ArithmetizationType> const &c) {
                jv = {
                        {"public_input_columns",             c.public_inputs()},
                        {"constant_columns",  c.constants()},
                        {"selector_columns",      c.selectors()},
                        {"_table_description", c.table_description()},
                        {"selector_map", c.get_selector_map()},
                        {"next_selector_index", c.get_next_selector_index()},
                        {"allocated_public_input_rows", c.get_allocated_public_input_rows()},
                        {"selector_index", c.get_selector_index()},
                };
            }

            template<typename ArithmetizationType>
            void tag_invoke(boost::json::value_from_tag, boost::json::value &jv,
                            zk::blueprint<ArithmetizationType> const &c) {
                jv = {
                        {"_gates",             c.gates()},
                        {"_copy_constraints",  c.copy_constraints()},
                        {"_lookup_gates",      c.lookup_gates()},
                        {"_table_description", c.table_description()}
                };
            }

            namespace snark {
                template<typename FieldType>
                void tag_invoke(boost::json::value_from_tag, boost::json::value &jv,
                                nil::crypto3::zk::snark::plonk_variable<FieldType> const &c) {
                    jv = {
                            {"rotation", c.rotation},
                            {"type",     (uint8_t) c.type},
                            {"index",    c.index},
                            {"relative", c.relative},
                    };
                }

                template<typename FieldType, typename ConstraintType>
                void tag_invoke(boost::json::value_from_tag, boost::json::value &jv,
                                nil::crypto3::zk::snark::plonk_gate<FieldType, ConstraintType> const &c) {
                    jv = {
                            {"selector_index", c.selector_index},
                            {"constraints",    c.constraints}
                    };
                }

                template<typename FieldType>
                void tag_invoke(boost::json::value_from_tag, boost::json::value &jv,
                                nil::crypto3::zk::snark::plonk_lookup_constraint<FieldType> const &c) {
                    jv = {
                            {"lookup_input", c.lookup_input},
                            {"lookup_value", c.lookup_value}
                    };
                }

                template<typename BlueprintFieldType, typename ArithmetizationParams>
                void tag_invoke(boost::json::value_from_tag, boost::json::value &jv,
                                nil::crypto3::zk::snark::plonk_table_description<BlueprintFieldType, ArithmetizationParams> const &c) {
                    jv = {
                            {"rows_amount",        c.rows_amount},
                            {"usable_rows_amount", c.usable_rows_amount},
                    };
                }

                template<typename AssignType, typename std::enable_if<nil::crypto3::detail::is_plonk_table<AssignType>::value, bool>::type = true>
                void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, AssignType const &c) {
                    jv = {
                            {"_private_table", {"witness_columns", c.private_table().witnesses()}},
                            {"_public_table",  {{"public_input_columns", c.public_table().public_inputs()},
                                                                   {"constant_columns", c.public_table().constants()},
                                                       {"selector_columns", c.public_table().selectors()},
                                               }},
                    };
                }

                template<typename PublicPreprocessedData, typename std::enable_if<nil::crypto3::detail::is_public_preprocessed_data_type<PublicPreprocessedData>::value, bool>::type = true>
                void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, PublicPreprocessedData const &c) {
                    jv = {
                            {"public_polynomial_table",
                                                        {{"public_input_columns", c.public_polynomial_table.public_inputs()},
                                                                {"constant_columns",  c.public_polynomial_table.constants()},
                                                                {"selector_columns", c.public_polynomial_table.selectors()}},
                            },
                            {"permutation_polynomials", c.permutation_polynomials},
                            {"identity_polynomials",    c.identity_polynomials},
                            {"q_last",                  c.q_last},
                            {"q_blind",                 c.q_blind},
                            {"precommitments",          {{"id_permutation",       c.precommitments.id_permutation},
                                                                {"sigma_permutation", c.precommitments.sigma_permutation},
                                                                {"public_input",     c.precommitments.public_input},
                                                                {"constant",    c.precommitments.constant},
                                                                {"selector",          c.precommitments.selector},
                                                                {"special_selectors", c.precommitments.special_selectors}}},
                            {"common_data",             {{"basic_domain",         c.common_data.basic_domain},
                                                                {"Z",                 c.common_data.Z},
                                                                {"lagrange_0",       c.common_data.lagrange_0},
                                                                {"commitments", {{"id_permutation", c.common_data.commitments.id_permutation},
                                                                                        {"sigma_permutation", c.common_data.commitments.sigma_permutation},
                                                                                        {"public_input", c.common_data.commitments.public_input},
                                                                                        {"constant", c.common_data.commitments.constant},
                                                                                        {"selector", c.common_data.commitments.selector},
                                                                                        {"special_selectors", c.common_data.commitments.special_selectors}}},
                                                                {"columns_rotations", c.common_data.columns_rotations},
                                                                {"rows_amount",       c.common_data.rows_amount}}
                            }
                    };
                }

                template<typename PrivatePreprocessedData, typename std::enable_if<nil::crypto3::detail::is_private_preprocessed_data_type<PrivatePreprocessedData>::value, bool>::type = true>
                void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, PrivatePreprocessedData const &c) {
                    jv = {
                            {"basic_domain",             c.basic_domain},
                            {"private_polynomial_table", {{"witness_columns", c.private_polynomial_table.witnesses()}}},
                    };
                }
            }

            namespace commitments {

                namespace detail {
                    template<typename ParamsType, typename std::enable_if<nil::crypto3::detail::is_fri_params<ParamsType>::value, bool>::type = true>
                    void tag_invoke(boost::json::value_from_tag, boost::json::value &jv,
                                    ParamsType const &c) {
                        jv = {
                                {"r",          c.r},
                                {"max_degree", c.max_degree},
                                {"D",          c.D},
                                {"step_list",  c.step_list}
                        };
                    }
                }
            }
        }

        namespace math {
            template<typename T>
            void tag_invoke(boost::json::value_from_tag, boost::json::value &jv,
                            nil::crypto3::math::non_linear_term<T> const &c) {
                jv = {
                        {"vars",  c.vars},
                        {"coeff", c.coeff}
                };
            }

            template<typename T>
            void tag_invoke(boost::json::value_from_tag, boost::json::value &jv,
                            nil::crypto3::math::non_linear_combination<T> const &c) {
                jv = {
                        {"terms", c.terms},
                };
            }

            template<typename FieldType, typename ValueType = typename FieldType::value_type>
            boost::json::value get_evaluation_domain_fields(std::size_t m,
                                                            std::shared_ptr<nil::crypto3::math::evaluation_domain<FieldType>> const &c) {
                boost::json::value jv;

                typedef std::shared_ptr<evaluation_domain<FieldType, ValueType>> result_type;

                const std::size_t big = 1ul << (std::size_t(std::ceil(std::log2(m))) - 1);
                const std::size_t rounded_small = (1ul << std::size_t(std::ceil(std::log2(m - big))));

                if (detail::is_basic_radix2_domain<FieldType>(m)) {
//                    basic_radix2_domain<FieldType, ValueType> *tmp =  (basic_radix2_domain<FieldType, ValueType> *) &c;
                    basic_radix2_domain<FieldType, ValueType> tmp(m);
                    jv = {{"root",              c->root},
                          {"root_inverse",      c->root_inverse},
                          {"domain",            c->domain},
                          {"domain_inverse",    c->domain_inverse},
                          {"generator",         c->generator},
                          {"generator_inverse", c->generator_inverse},
                          {"m",                 c->m},
                          {"log2_size",         c->log2_size},
                          {"generator_size",    c->generator_size},
                          {"omega",             tmp.omega}};
                    return jv;
                }

                if (detail::is_extended_radix2_domain<FieldType>(m)) {
//                    extended_radix2_domain<FieldType, ValueType> *tmp =  (extended_radix2_domain<FieldType, ValueType> *) &c;
                    extended_radix2_domain<FieldType, ValueType> tmp(m);
                    jv = {{"root",              c->root},
                          {"root_inverse",      c->root_inverse},
                          {"domain",            c->domain},
                          {"domain_inverse",    c->domain_inverse},
                          {"generator",         c->generator},
                          {"generator_inverse", c->generator_inverse},
                          {"m",                 c->m},
                          {"log2_size",         c->log2_size},
                          {"generator_size",    c->generator_size},
                          {"small_m",           tmp.small_m},
                          {"omega",             tmp.omega},
                          {"shift",             tmp.shift}};
                    return jv;
                }

                if (detail::is_step_radix2_domain<FieldType>(m)) {
//                    step_radix2_domain<FieldType, ValueType> *tmp =  (step_radix2_domain<FieldType, ValueType> *) &c;
                    step_radix2_domain<FieldType, ValueType> tmp(m);
                    jv = {{"root",              c->root},
                          {"root_inverse",      c->root_inverse},
                          {"domain",            c->domain},
                          {"domain_inverse",    c->domain_inverse},
                          {"generator",         c->generator},
                          {"generator_inverse", c->generator_inverse},
                          {"m",                 c->m},
                          {"log2_size",         c->log2_size},
                          {"generator_size",    c->generator_size},
                          {"big_m",             tmp.big_m},
                          {"small_m",           tmp.small_m},
                          {"omega",             tmp.omega},
                          {"big_omega",         tmp.big_omega},
                          {"small_omega",       tmp.small_omega}};
                    return jv;
                }

                if (detail::is_basic_radix2_domain<FieldType>(big + rounded_small)) {
//                    basic_radix2_domain<FieldType, ValueType> *tmp =  (basic_radix2_domain<FieldType, ValueType> *) &c;
                    basic_radix2_domain<FieldType, ValueType> tmp(big + rounded_small);
                    jv = {{"root",              c->root},
                          {"root_inverse",      c->root_inverse},
                          {"domain",            c->domain},
                          {"domain_inverse",    c->domain_inverse},
                          {"generator",         c->generator},
                          {"generator_inverse", c->generator_inverse},
                          {"m",                 c->m},
                          {"log2_size",         c->log2_size},
                          {"generator_size",    c->generator_size},
                          {"omega",             tmp.omega}};
                    return jv;
                }

                if (detail::is_extended_radix2_domain<FieldType>(big + rounded_small)) {
//                    extended_radix2_domain<FieldType, ValueType> *tmp =  (extended_radix2_domain<FieldType, ValueType> *) &c;
                    extended_radix2_domain<FieldType, ValueType> tmp(big + rounded_small);
                    jv = {{"root",              c->root},
                          {"root_inverse",      c->root_inverse},
                          {"domain",            c->domain},
                          {"domain_inverse",    c->domain_inverse},
                          {"generator",         c->generator},
                          {"generator_inverse", c->generator_inverse},
                          {"m",                 c->m},
                          {"log2_size",         c->log2_size},
                          {"generator_size",    c->generator_size},
                          {"small_m",           tmp.small_m},
                          {"omega",             tmp.omega},
                          {"shift",             tmp.shift}};
                    return jv;
                }

                if (detail::is_step_radix2_domain<FieldType>(big + rounded_small)) {
//                    step_radix2_domain<FieldType, ValueType> *tmp =  (step_radix2_domain<FieldType, ValueType> *) &c;
                    step_radix2_domain<FieldType, ValueType> tmp(big + rounded_small);
                    jv = {{"root",              c->root},
                          {"root_inverse",      c->root_inverse},
                          {"domain",            c->domain},
                          {"domain_inverse",    c->domain_inverse},
                          {"generator",         c->generator},
                          {"generator_inverse", c->generator_inverse},
                          {"m",                 c->m},
                          {"log2_size",         c->log2_size},
                          {"generator_size",    c->generator_size},
                          {"big_m",             tmp.big_m},
                          {"small_m",           tmp.small_m},
                          {"omega",             tmp.omega},
                          {"big_omega",         tmp.big_omega},
                          {"small_omega",       tmp.small_omega}};
                    return jv;
                }

                if (detail::is_geometric_sequence_domain<FieldType>(m)) {
//                    geometric_sequence_domain<FieldType, ValueType> *tmp =  (geometric_sequence_domain<FieldType, ValueType> *) &c;
                    geometric_sequence_domain<FieldType, ValueType> tmp(m);
                    jv = {{"root",                          c->root},
                          {"root_inverse",                  c->root_inverse},
                          {"domain",                        c->domain},
                          {"domain_inverse",                c->domain_inverse},
                          {"generator",                     c->generator},
                          {"generator_inverse",             c->generator_inverse},
                          {"m",                             c->m},
                          {"log2_size",                     c->log2_size},
                          {"generator_size",                c->generator_size},
                          {"precomputation_sentinel",       tmp.precomputation_sentinel},
                          {"geometric_sequence",            tmp.geometric_sequence},
                          {"geometric_triangular_sequence", tmp.geometric_triangular_sequence}};
                    return jv;
                }

                if (detail::is_arithmetic_sequence_domain<FieldType>(m)) {
//                    arithmetic_sequence_domain<FieldType, ValueType> *tmp =  (arithmetic_sequence_domain<FieldType, ValueType> *) &c;
                    arithmetic_sequence_domain<FieldType, ValueType> tmp(m);
                    jv = {{"root",                    c->root},
                          {"root_inverse",            c->root_inverse},
                          {"domain",                  c->domain},
                          {"domain_inverse",          c->domain_inverse},
                          {"generator",               c->generator},
                          {"generator_inverse",       c->generator_inverse},
                          {"m",                       c->m},
                          {"log2_size",               c->log2_size},
                          {"generator_size",          c->generator_size},
                          {"precomputation_sentinel", tmp.precomputation_sentinel},
                          {"subproduct_tree",         tmp.subproduct_tree},
                          {"arithmetic_sequence",     tmp.arithmetic_sequence},
                          {"arithmetic_generator",    tmp.arithmetic_generator}};

                    return jv;
                }

                return jv;
            }

            template<typename T>
            void tag_invoke(boost::json::value_from_tag, boost::json::value &jv,
                            std::shared_ptr<nil::crypto3::math::evaluation_domain<T>> const &c) {
                jv = {
                        get_evaluation_domain_fields(c->m, c)
//                        {"root",              c->root},
//                        {"root_inverse",      c->root_inverse},
//                        {"domain",            c->domain},
//                        {"domain_inverse",    c->domain_inverse},
//                        {"generator",         c->generator},
//                        {"generator_inverse", c->generator_inverse},
//                        {"m",                 c->m},
//                        {"log2_size",         c->log2_size},
//                        {"generator_size",    c->generator_size},
// it's fields in evalution_domain
                };
            }

            template<typename T>
            void tag_invoke(boost::json::value_from_tag, boost::json::value &jv,
                            nil::crypto3::math::polynomial_dfs<T> const &c) {
                jv = {
                        {"val", std::vector<T>(c.begin(), c.end())},
                        {"_d",  c.degree()},
                };
            }

            template<typename T>
            void tag_invoke(boost::json::value_from_tag, boost::json::value &jv,
                            nil::crypto3::math::polynomial<T> const &c) {
                jv = {{"val", std::vector<T>(c.begin(), c.end())}};
            }
        };

        namespace algebra {
            namespace fields {
                namespace detail {
                    template<typename T>
                    void tag_invoke(boost::json::value_from_tag, boost::json::value &jv,
                                    nil::crypto3::algebra::fields::detail::element_fp<T> const &c) {
                        std::stringstream st;
                        st << std::hex << c.data;
                        jv = "0x" + st.str();
                    }
                }
            }
        }

        namespace containers {
            namespace detail {
                template<typename NodeType, std::size_t Arity>
                void tag_invoke(boost::json::value_from_tag, boost::json::value &jv,
                                crypto3::containers::detail::merkle_tree_impl<NodeType, Arity> const &c) {
                    typedef typename NodeType::value_type value_type;
                    typedef std::vector<value_type> container_type;
                    jv = {
                            {"_hashes", container_type(c.begin(), c.end())},
                            {"_leaves", c.leaves()},
                            {"_size",   c.complete_size()},
                            {"_rc",     c.row_count()},
                    };
                }
            }
        }
    }
}
#endif //PLACEHOLDER_PROOF_GEN_SERIALIZATION_STRUCTS_HPP
