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

#ifndef PLACEHOLDER_PROOF_GEN_DESERIALIZATION_STRUCTS_HPP
#define PLACEHOLDER_PROOF_GEN_DESERIALIZATION_STRUCTS_HPP


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
            template<typename BlueprintFieldType,
                    typename ArithmetizationParams>
            zk::blueprint_private_assignment_table<zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>> generate_private_tmp(const boost::json::value &jv, zk::snark::plonk_table_description<BlueprintFieldType, ArithmetizationParams> &_table_description) {
                std::size_t i = 0;
                std::array<zk::snark::plonk_column<typename BlueprintFieldType::value_type>, ArithmetizationParams::witness_columns> witness_columns;
                for (const boost::json::value &val: jv.at("witness_columns").as_array()) {
                    witness_columns[i] = boost::json::value_to<std::vector<typename BlueprintFieldType::value_type>>(val);
                    ++i;
                }
                snark::plonk_private_assignment_table<BlueprintFieldType,
                        ArithmetizationParams> x(witness_columns);

                return zk::blueprint_private_assignment_table<zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>>(x, _table_description);
            }

            template<typename BlueprintFieldType,
                    typename ArithmetizationParams>
            zk::blueprint_public_assignment_table<zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>> generate_tmp(const boost::json::value &jv, zk::snark::plonk_table_description<BlueprintFieldType, ArithmetizationParams> &_table_description) {
                std::size_t i = 0;
                std::array<std::vector<typename BlueprintFieldType::value_type>, ArithmetizationParams::public_input_columns> public_input_columns;
                for (const boost::json::value &val: jv.at("public_input_columns").as_array()) {
                    public_input_columns[i] = boost::json::value_to<std::vector<typename BlueprintFieldType::value_type>>(val);
                    ++i;
                }

                i = 0;
                std::array<std::vector<typename BlueprintFieldType::value_type>, ArithmetizationParams::constant_columns> constant_columns;
                for (const boost::json::value &val: jv.at("constant_columns").as_array()) {
                    constant_columns[i] = boost::json::value_to<std::vector<typename BlueprintFieldType::value_type>>(val);
                    ++i;
                }

                i = 0;
                std::array<std::vector<typename BlueprintFieldType::value_type>, ArithmetizationParams::selector_columns> selector_columns;
                for (const boost::json::value &val: jv.at("selector_columns").as_array()) {
                    selector_columns[i] = boost::json::value_to<std::vector<typename BlueprintFieldType::value_type>>(val);
                    ++i;
                }

                std::map<std::size_t, std::size_t> selector_map;
                for (const boost::json::value &val: jv.at("selector_map").as_array()) {
                    selector_map.insert(std::pair<std::size_t, std::size_t>(boost::json::value_to<std::size_t>(val.at(0)), boost::json::value_to<std::size_t>(val.at(1))));
                }

                std::size_t next_selector_index  =  boost::json::value_to<std::size_t>(jv.at("next_selector_index"));
                std::size_t allocated_public_input_rows  =  boost::json::value_to<std::size_t>(jv.at("allocated_public_input_rows"));
                std::size_t selector_index  =  boost::json::value_to<std::size_t>(jv.at("selector_index"));

                return zk::blueprint_public_assignment_table<zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>>(public_input_columns, constant_columns, selector_columns, _table_description, selector_map, next_selector_index, allocated_public_input_rows, selector_index);
            }

            namespace snark {
                template<typename BlueprintFieldType, typename ArithmetizationParams>
                snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>
                tag_invoke(boost::json::value_to_tag<snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>>,
                           const boost::json::value &jv) {
                    typedef std::vector<zk::snark::plonk_gate<BlueprintFieldType, zk::snark::plonk_constraint<BlueprintFieldType>>> gates_type;
                    typedef std::vector<zk::snark::plonk_copy_constraint<BlueprintFieldType>> copy_constraints_type;
                    typedef std::vector<zk::snark::plonk_gate<BlueprintFieldType, zk::snark::plonk_lookup_constraint<BlueprintFieldType>>> lookup_gates_type;

                    gates_type x = boost::json::value_to<gates_type>(jv.at("_gates"));
                    copy_constraints_type y = boost::json::value_to<copy_constraints_type>(jv.at("_copy_constraints"));
                    lookup_gates_type z = boost::json::value_to<lookup_gates_type>(jv.at("_lookup_gates"));

                    snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams> ArithmetizationType(x, y, z);
                    return ArithmetizationType;
                }

                template<typename BlueprintFieldType, typename ArithmetizationParams>
                snark::plonk_public_assignment_table<BlueprintFieldType, ArithmetizationParams>
                tag_invoke(boost::json::value_to_tag<snark::plonk_public_assignment_table<BlueprintFieldType, ArithmetizationParams>>,
                           const boost::json::value &jv) {
                    std::size_t i = 0;
                    std::array<std::vector<typename BlueprintFieldType::value_type>, ArithmetizationParams::public_input_columns> public_input_columns;
                    for (const boost::json::value &val: jv.at("public_input_columns").as_array()) {
                        public_input_columns[i] = boost::json::value_to<std::vector<typename BlueprintFieldType::value_type>>(val);
                        ++i;
                    }

                    i = 0;
                    std::array<std::vector<typename BlueprintFieldType::value_type>, ArithmetizationParams::public_input_columns> constant_columns;
                    for (const boost::json::value &val: jv.at("constant_columns").as_array()) {
                        constant_columns[i] = boost::json::value_to<std::vector<typename BlueprintFieldType::value_type>>(val);
                        ++i;
                    }

                    i = 0;
                    std::array<std::vector<typename BlueprintFieldType::value_type>, ArithmetizationParams::public_input_columns> selector_columns;
                    for (const boost::json::value &val: jv.at("selector_columns").as_array()) {
                        selector_columns[i] = boost::json::value_to<std::vector<typename BlueprintFieldType::value_type>>(val);
                        ++i;
                    }
                    snark::plonk_public_assignment_table<BlueprintFieldType, ArithmetizationParams> tmp(public_input_columns, constant_columns, selector_columns);
                    return tmp;
                }

                template<typename BlueprintFieldType, typename ArithmetizationParams>
                zk::snark::plonk_table_description<BlueprintFieldType, ArithmetizationParams>
                tag_invoke(boost::json::value_to_tag<zk::snark::plonk_table_description<BlueprintFieldType, ArithmetizationParams>>,
                           const boost::json::value &jv) {
                    return {
                            .rows_amount = boost::json::value_to<std::size_t>(jv.at("rows_amount")),
                            .usable_rows_amount = boost::json::value_to<std::size_t>(jv.at("usable_rows_amount"))
                    };
                }

                template<typename FieldType, typename ConstraintType>
                nil::crypto3::zk::snark::plonk_gate<FieldType, ConstraintType> tag_invoke(boost::json::value_to_tag<nil::crypto3::zk::snark::plonk_gate<FieldType, ConstraintType>>,
                                                                                          const boost::json::value &jv) {
                    nil::crypto3::zk::snark::plonk_gate<FieldType, ConstraintType> x(boost::json::value_to<std::size_t>(jv.at("selector_index")),
                                                                                     boost::json::value_to<std::vector<ConstraintType>>(jv.at("constraints")));
                    return x;
                }
            }
        }

        namespace zk {
            namespace snark {
                template<typename FieldType>
                nil::crypto3::zk::snark::plonk_variable<FieldType>
                tag_invoke(boost::json::value_to_tag<nil::crypto3::zk::snark::plonk_variable<FieldType>>,
                           const boost::json::value &jv) {
                    std::size_t type_int = boost::json::value_to<std::size_t>(jv.at("type"));
                    typename nil::crypto3::zk::snark::plonk_variable<FieldType>::column_type type;
                    if (type_int == 0) {
                        type = nil::crypto3::zk::snark::plonk_variable<FieldType>::column_type::witness;
                    }
                    if (type_int == 1) {
                        type = nil::crypto3::zk::snark::plonk_variable<FieldType>::column_type::public_input;
                    }
                    if (type_int == 2) {
                        type = nil::crypto3::zk::snark::plonk_variable<FieldType>::column_type::constant;
                    }
                    if (type_int == 3) {
                        type = nil::crypto3::zk::snark::plonk_variable<FieldType>::column_type::selector;
                    }
                    nil::crypto3::zk::snark::plonk_variable<FieldType> tmp(
                            boost::json::value_to<std::size_t>(jv.at("index")),
                            boost::json::value_to<int>(jv.at("rotation")),
                            boost::json::value_to<bool>(jv.at("relative")),
                            type);

                    return tmp;
                }

                template<typename FieldType>
                typename nil::crypto3::zk::snark::plonk_variable<FieldType>::assignment_type
                tag_invoke(boost::json::value_to_tag<typename nil::crypto3::zk::snark::plonk_variable<FieldType>::assignment_type>,
                           const boost::json::value &jv) {
                    typename nil::crypto3::zk::snark::plonk_variable<FieldType>::assignment_type tmp;
                    std::cout << boost::json::serialize(jv) << std::endl;
                    return tmp;
                    // todo: zero
                }

                template<typename FieldType>
                nil::crypto3::zk::snark::plonk_copy_constraint<FieldType>
                tag_invoke(boost::json::value_to_tag<nil::crypto3::zk::snark::plonk_copy_constraint<FieldType>>,
                           const boost::json::value &jv) {
                    nil::crypto3::zk::snark::plonk_copy_constraint<FieldType> x;
                    x.first = boost::json::value_to<nil::crypto3::zk::snark::plonk_variable<FieldType>>(jv.at(0));
                    x.second = boost::json::value_to<nil::crypto3::zk::snark::plonk_variable<FieldType>>(jv.at(1));
                    return x;
                }


                template<typename FieldType>
                nil::crypto3::zk::snark::plonk_lookup_constraint<FieldType> tag_invoke(boost::json::value_to_tag<nil::crypto3::zk::snark::plonk_lookup_constraint<FieldType>>,
                                                                                       const boost::json::value &jv) {
                    std::cout << boost::json::serialize(jv) << std::endl;
                    nil::crypto3::zk::snark::plonk_lookup_constraint<FieldType> tmp;
                    return tmp;
                    // todo: bad part need fill
                }
            }
        }

        namespace math {
            template<typename T>
            nil::crypto3::math::non_linear_term<nil::crypto3::zk::snark::plonk_variable<T>> tag_invoke(boost::json::value_to_tag<nil::crypto3::math::non_linear_term<nil::crypto3::zk::snark::plonk_variable<T>>>, const boost::json::value &jv) {
                nil::crypto3::math::non_linear_term<nil::crypto3::zk::snark::plonk_variable<T>> tmp(boost::json::value_to<std::vector<nil::crypto3::zk::snark::plonk_variable<T>>>(jv.at("vars")),
                                                                                                    boost::json::value_to<typename nil::crypto3::zk::snark::plonk_variable<T>::assignment_type>(jv.at("coeff")));
                return tmp;
            }

            template<typename BaseField>
            nil::crypto3::zk::snark::plonk_constraint<BaseField> tag_invoke(boost::json::value_to_tag<nil::crypto3::zk::snark::plonk_constraint<BaseField>>,
                                                                            const boost::json::value &jv) {
                std::vector<nil::crypto3::math::non_linear_term<nil::crypto3::zk::snark::plonk_variable<BaseField>>> terms = boost::json::value_to<std::vector<nil::crypto3::math::non_linear_term<nil::crypto3::zk::snark::plonk_variable<BaseField>>>>(jv.at("terms"));
                nil::crypto3::zk::snark::plonk_constraint<BaseField> x(terms);
                return x;
            }

            template<typename T>
            nil::crypto3::math::non_linear_combination<T> tag_invoke(boost::json::value_to_tag<nil::crypto3::math::non_linear_combination<T>>,
                                                                     const boost::json::value &jv) {
                nil::crypto3::math::non_linear_combination<T> x = boost::json::value_to<nil::crypto3::math::non_linear_combination<T>>(jv.at("terms"));
                return x;
            }
        };

        namespace algebra {
            namespace fields {
                namespace detail {
                    template<typename T>
                    nil::crypto3::algebra::fields::detail::element_fp<T> tag_invoke(boost::json::value_to_tag<nil::crypto3::algebra::fields::detail::element_fp<T>>,
                                                                                    const boost::json::value &jv) {

                        multiprecision::cpp_int x(boost::json::value_to<std::string>(jv));
                        return nil::crypto3::algebra::fields::detail::element_fp<T>(x);
                    }
                }
            }
        }
    }
}
#endif //PLACEHOLDER_PROOF_GEN_DESERIALIZATION_STRUCTS_HPP
