//---------------------------------------------------------------------------//
// Copyright (c) 2021-2022 Mikhail Komarov <nemo@nil.foundation>
// Copyright (c) 2021-2022 Nikita Kaskov <nbering@nil.foundation>
// Copyright (c) 2022 Alisa Cherniaeva <a.cherniaeva@nil.foundation>
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

#define BOOST_TEST_MODULE zerg_plonk_unified_addition_test

#include <boost/test/unit_test.hpp>

#include <nil/crypto3/algebra/curves/pallas.hpp>
#include <nil/crypto3/algebra/fields/arithmetic_params/pallas.hpp>
#include <nil/crypto3/algebra/random_element.hpp>

#include <nil/crypto3/hash/algorithm/hash.hpp>
#include <nil/crypto3/hash/sha2.hpp>
#include <nil/crypto3/hash/keccak.hpp>

#include <nil/crypto3/zk/snark/arithmetization/plonk/params.hpp>

#include <nil/crypto3/zk/blueprint/plonk.hpp>
#include <nil/crypto3/zk/assignment/plonk.hpp>
#include <nil/crypto3/zk/components/algebra/curves/pasta/plonk/unified_addition.hpp>

#include "test_plonk_component.hpp"

#include <nil/crypto3/zk/snark/arithmetization/plonk/table_description.hpp>

#include <nil/crypto3/placeholder-proof-gen/json_serialization.hpp>
#include <nil/crypto3/placeholder-proof-gen/json_deserialization.hpp>

#include <nil/crypto3/algebra/curves/ed25519.hpp>
#include <nil/crypto3/algebra/fields/arithmetic_params/ed25519.hpp>
#include <nil/crypto3/zk/components/non_native/algebra/fields/plonk/signatures_verification.hpp>
#include <nil/crypto3/zk/components/non_native/algebra/fields/plonk/ed25519.hpp>

// scalar_mul
#include <nil/crypto3/zk/components/algebra/curves/pasta/plonk/variable_base_scalar_mul_15_wires.hpp>
// end

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

using namespace nil::crypto3;


template <typename BlueprintFieldType, typename ArithmetizationParams, typename Hash,
        std::size_t Lambda>
void check_enough(zk::snark::plonk_table_description<BlueprintFieldType, ArithmetizationParams> desc, zk::blueprint<zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>> bp) {
    using ArithmetizationType = zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>;
    zk::blueprint_private_assignment_table<ArithmetizationType> private_assignment(desc);
    zk::blueprint_public_assignment_table<ArithmetizationType> public_assignment(desc);

    std::cout << "Usable rows: " << desc.usable_rows_amount << std::endl;
    std::cout << "Padded rows: " << desc.rows_amount << std::endl;

    zk::snark::plonk_assignment_table<BlueprintFieldType, ArithmetizationParams> assignments(private_assignment,
                                                                                             public_assignment);

    using placeholder_params =
            zk::snark::placeholder_params<BlueprintFieldType, ArithmetizationParams, Hash, Hash, Lambda>;
    using types = zk::snark::detail::placeholder_policy<BlueprintFieldType, placeholder_params>;

    using fri_type =
            typename zk::commitments::fri<BlueprintFieldType, typename placeholder_params::merkle_hash_type,
                    typename placeholder_params::transcript_hash_type, 2, 1>;

    std::size_t table_rows_log = std::ceil(std::log2(desc.rows_amount));

    typename fri_type::params_type fri_params = create_fri_params<fri_type, BlueprintFieldType>(table_rows_log);

    std::size_t permutation_size = desc.witness_columns + desc.public_input_columns + desc.constant_columns;

    typename zk::snark::placeholder_public_preprocessor<
            BlueprintFieldType, placeholder_params>::preprocessed_data_type public_preprocessed_data =
            zk::snark::placeholder_public_preprocessor<BlueprintFieldType, placeholder_params>::process(
                    bp, public_assignment, desc, fri_params, permutation_size);
    typename zk::snark::placeholder_private_preprocessor<
            BlueprintFieldType, placeholder_params>::preprocessed_data_type private_preprocessed_data =
            zk::snark::placeholder_private_preprocessor<BlueprintFieldType, placeholder_params>::process(
                    bp, private_assignment, desc, fri_params);
}


BOOST_AUTO_TEST_SUITE(blueprint_plonk_test_suite)

    BOOST_AUTO_TEST_CASE(parsing_json) {
        std::string json_st;
        std::string FILENAME = "unified_addition_2.data";
        boost::json::error_code ec;
        if (boost::filesystem::exists(FILENAME)) {
            boost::filesystem::load_string_file(FILENAME, json_st);
        }
        boost::json::value jv = boost::json::parse( json_st, ec );
        if( ec )
            std::cout << "Parsing failed: " << ec.message() << "\n";
        std::cout << jv.at("curve_type").as_string() << std::endl;
        placeholder_proof_gen::arithmetization_params_t arithmetization_params(jv.at("arithmetization_params"));
        if (arithmetization_params == placeholder_proof_gen::arithmetization_params_t(11, 1, 1, 1)) {
            using curve_type = algebra::curves::pallas;
            using BlueprintFieldType = typename curve_type::base_field_type;
            constexpr std::size_t WitnessColumns = 11;
            constexpr std::size_t PublicInputColumns = 1;
            constexpr std::size_t ConstantColumns = 1;
            constexpr std::size_t SelectorColumns = 1;
            using ArithmetizationParams =
                    zk::snark::plonk_arithmetization_params<WitnessColumns, PublicInputColumns, ConstantColumns, SelectorColumns>;
            using ArithmetizationType = zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>;
            using ArithmetizationType = zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>;
            using AssignmentType = zk::blueprint_assignment_table<ArithmetizationType>;
            using hash_type = nil::crypto3::hashes::keccak_1600<256>;
            constexpr std::size_t Lambda = 40;

            using var = zk::snark::plonk_variable<BlueprintFieldType>;

            using component_type = zk::components::curve_element_unified_addition<ArithmetizationType, curve_type, 0, 1, 2, 3,
                    4, 5, 6, 7, 8, 9, 10>;

            auto P = algebra::random_element<curve_type::template g1_type<>>().to_affine();
            auto Q = algebra::random_element<curve_type::template g1_type<>>().to_affine();
            typename curve_type::template g1_type<algebra::curves::coordinates::affine>::value_type zero = {0, 0};
            typename curve_type::template g1_type<algebra::curves::coordinates::affine>::value_type expected_res;
            P.X = Q.X;
            P.Y = -Q.Y;
            if (Q.X == zero.X && Q.Y == zero.Y) {
                expected_res = P;
            } else {
                if (P.X == zero.X && P.Y == zero.Y) {
                    expected_res = Q;
                } else {
                    if (P.X == Q.X && P.Y == -Q.Y) {
                        expected_res = {0, 0};
                    } else {
                        expected_res = P + Q;
                    }
                }
            }
            typename component_type::params_type params = {
                    {var(0, 0, false, var::column_type::public_input), var(0, 1, false, var::column_type::public_input)},
                    {var(0, 2, false, var::column_type::public_input), var(0, 3, false, var::column_type::public_input)}};

            std::vector<typename BlueprintFieldType::value_type> public_input = {P.X, P.Y, Q.X, Q.Y};

            auto result_check = [&expected_res](AssignmentType &assignment,
                                                component_type::result_type &real_res) {
                assert(expected_res.X == assignment.var_value(real_res.X));
                assert(expected_res.Y == assignment.var_value(real_res.Y));
            };

            test_component<component_type, BlueprintFieldType, ArithmetizationParams, hash_type, Lambda>(params, public_input, result_check);
        }
        if (arithmetization_params == placeholder_proof_gen::arithmetization_params_t(15, 1, 1, 3)) {
            using curve_type = algebra::curves::pallas;
            using BlueprintFieldType = typename curve_type::base_field_type;
            using BlueprintScalarType = typename curve_type::scalar_field_type;
            constexpr std::size_t WitnessColumns = 15;
            constexpr std::size_t PublicInputColumns = 1;
            constexpr std::size_t ConstantColumns = 1;
            constexpr std::size_t SelectorColumns = 3;
            using ArithmetizationParams = zk::snark::plonk_arithmetization_params<WitnessColumns,
                    PublicInputColumns, ConstantColumns, SelectorColumns>;
            using ArithmetizationType = zk::snark::plonk_constraint_system<BlueprintFieldType,
                    ArithmetizationParams>;
            using AssignmentType = zk::blueprint_assignment_table<ArithmetizationType>;
            using hash_type = nil::crypto3::hashes::keccak_1600<256>;
            constexpr std::size_t Lambda = 1;
            using component_type = zk::components::curve_element_variable_base_scalar_mul<ArithmetizationType, curve_type,
                    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14>;
            using var = zk::snark::plonk_variable<BlueprintFieldType>;
            typename BlueprintScalarType::value_type b_scalar = algebra::random_element<BlueprintScalarType>();

            typename curve_type::scalar_field_type::value_type shift_base = 2;
            auto shift = shift_base.pow(255) + 1;
            typename BlueprintScalarType::value_type x = (b_scalar - shift)/2;
            typename BlueprintScalarType::integral_type integral_x = typename BlueprintScalarType::integral_type(x.data);
            BlueprintFieldType::value_type x_scalar =  integral_x;


            curve_type::template g1_type<algebra::curves::coordinates::affine>::value_type T = algebra::random_element<curve_type::template g1_type<algebra::curves::coordinates::affine>>();
            var scalar_var = {0, 2, false, var::column_type::public_input};
            var T_X_var = {0, 0, false, var::column_type::public_input};
            var T_Y_var = {0, 1, false, var::column_type::public_input};
            typename component_type::params_type assignment_params = {{T_X_var, T_Y_var},scalar_var};
            std::vector<typename BlueprintFieldType::value_type> public_input = {T.X, T.Y, x_scalar};
            curve_type::template g1_type<algebra::curves::coordinates::affine>::value_type expected;
            if (b_scalar != 0) {
                expected = b_scalar * T;
            } else {
                expected = {0, 0};
            }
            auto result_check = [&expected, T, shift_base](AssignmentType &assignment,
                                                           component_type::result_type &real_res) {
                curve_type::template g1_type<algebra::curves::coordinates::affine>::value_type R;
                R.X = assignment.var_value(real_res.X);
                R.Y = assignment.var_value(real_res.Y);
                std::cout<<"Component result: "<< assignment.var_value(real_res.X).data <<" " << assignment.var_value(real_res.Y).data<<std::endl;
                assert(expected.X == assignment.var_value(real_res.X));
                assert(expected.Y == assignment.var_value(real_res.Y));
            };
            test_component<component_type, BlueprintFieldType, ArithmetizationParams, hash_type, Lambda>(assignment_params, public_input, result_check);
        }

    }


BOOST_AUTO_TEST_SUITE_END()