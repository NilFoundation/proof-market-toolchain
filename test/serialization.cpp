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

#define BOOST_TEST_MODULE serialization_test

#include <boost/test/unit_test.hpp>

#include <nil/crypto3/algebra/curves/pallas.hpp>
#include <nil/crypto3/algebra/fields/arithmetic_params/pallas.hpp>
#include <nil/crypto3/algebra/curves/vesta.hpp>
#include <nil/crypto3/algebra/fields/arithmetic_params/vesta.hpp>
#include <nil/crypto3/algebra/random_element.hpp>

#include <nil/crypto3/hash/algorithm/hash.hpp>
#include <nil/crypto3/hash/sha2.hpp>
#include <nil/crypto3/hash/keccak.hpp>

#include <nil/crypto3/zk/snark/arithmetization/plonk/params.hpp>

#include <nil/crypto3/zk/blueprint/plonk.hpp>
#include <nil/crypto3/zk/assignment/plonk.hpp>
#include <nil/crypto3/zk/components/algebra/curves/pasta/plonk/variable_base_scalar_mul_15_wires.hpp>

// ---
#include <nil/crypto3/placeholder-proof-gen/json_serialization.hpp>
#include <nil/crypto3/zk/algorithms/allocate.hpp>

using namespace nil::crypto3;


BOOST_AUTO_TEST_SUITE(blueprint_plonk_test_suite)

    BOOST_AUTO_TEST_CASE(blueprint_plonk_unified_addition_double) {

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
        std::cout<<"Expected result: "<< expected.X.data <<" " << expected.Y.data<<std::endl;
        auto result_check = [&expected, T, shift_base](AssignmentType &assignment,
                                                       component_type::result_type &real_res) {
            curve_type::template g1_type<algebra::curves::coordinates::affine>::value_type R;
            R.X = assignment.var_value(real_res.X);
            R.Y = assignment.var_value(real_res.Y);
            std::cout<<"Component result: "<< assignment.var_value(real_res.X).data <<" " << assignment.var_value(real_res.Y).data<<std::endl;
            assert(expected.X == assignment.var_value(real_res.X));
            assert(expected.Y == assignment.var_value(real_res.Y));
        };
        //
        std::string signature;

        using ArithmetizationType = zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>;
        using component_type = component_type;

        zk::snark::plonk_table_description<BlueprintFieldType, ArithmetizationParams> desc;

        zk::blueprint<ArithmetizationType> bp(desc);
        zk::blueprint_private_assignment_table<ArithmetizationType> private_assignment(desc);
        zk::blueprint_public_assignment_table<ArithmetizationType> public_assignment(desc);
        zk::blueprint_assignment_table<ArithmetizationType> assignment_bp(private_assignment, public_assignment);

        std::size_t start_row = zk::components::allocate<component_type>(bp);

        zk::components::generate_circuit<component_type>(bp, public_assignment, assignment_params, start_row);

        boost::json::value jv = {
                {"curve_type", "pallas"},
                {"lambda", Lambda},
                {"hash", "keccak_1600<256>"},
                {"arithmetization_params", {{"witness_columns", WitnessColumns}, {"public_input_columns", PublicInputColumns},{"constant_columns", ConstantColumns},{"selector_columns", SelectorColumns}}},
                {"public_assignment",         public_assignment},
                {"desc",                      desc},
                {"bp",                        bp},
        };
        placeholder_proof_gen::pretty_print(std::cout, jv);
    }

BOOST_AUTO_TEST_SUITE_END()