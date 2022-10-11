//---------------------------------------------------------------------------//
// Copyright (c) 2022 Mikhail Komarov <nemo@nil.foundation>
// Copyright (c) 2022 Aleksei Moskvin <alalmoskvin@gmail.com>
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
#include <nil/crypto3/algebra/curves/pallas.hpp>
#include <nil/crypto3/algebra/fields/arithmetic_params/pallas.hpp>

#include <nil/crypto3/algebra/curves/curve25519.hpp>
#include <nil/crypto3/algebra/fields/arithmetic_params/ed25519.hpp>

#include <nil/crypto3/hash/algorithm/hash.hpp>
#include <nil/crypto3/hash/sha2.hpp>
#include <nil/crypto3/hash/keccak.hpp>

#include <nil/crypto3/zk/snark/arithmetization/plonk/params.hpp>

#include <nil/crypto3/zk/blueprint/plonk.hpp>
#include <nil/crypto3/zk/assignment/plonk.hpp>
#include <nil/crypto3/zk/components/non_native/algebra/fields/plonk/multiplication.hpp>

#include <nil/crypto3/placeholder-proof-gen/json_serialization.hpp>
#include <nil/crypto3/zk/algorithms/allocate.hpp>
#include <fstream>

using namespace nil::crypto3;

int main() {
    using curve_type = algebra::curves::pallas;
    using ed25519_type = algebra::curves::curve25519;
    using BlueprintFieldType = typename curve_type::base_field_type;
    constexpr std::size_t WitnessColumns = 9;
    constexpr std::size_t PublicInputColumns = 1;
    constexpr std::size_t ConstantColumns = 0;
    constexpr std::size_t SelectorColumns = 2;
    using ArithmetizationParams =
        zk::snark::plonk_arithmetization_params<WitnessColumns, PublicInputColumns, ConstantColumns, SelectorColumns>;
    using ArithmetizationType = zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>;
    using AssignmentType = zk::blueprint_assignment_table<ArithmetizationType>;
    using hash_type = nil::crypto3::hashes::keccak_1600<256>;
    constexpr std::size_t Lambda = 1;

    using var = zk::snark::plonk_variable<BlueprintFieldType>;

    using component_type = zk::components::non_native_field_element_multiplication<ArithmetizationType,
                                                                                   curve_type,
                                                                                   ed25519_type,
                                                                                   0,
                                                                                   1,
                                                                                   2,
                                                                                   3,
                                                                                   4,
                                                                                   5,
                                                                                   6,
                                                                                   7,
                                                                                   8>;

    std::array<var, 4> input_var_a = {
        var(0, 0, false, var::column_type::public_input), var(0, 1, false, var::column_type::public_input),
        var(0, 2, false, var::column_type::public_input), var(0, 3, false, var::column_type::public_input)};
    std::array<var, 4> input_var_b = {
        var(0, 4, false, var::column_type::public_input), var(0, 5, false, var::column_type::public_input),
        var(0, 6, false, var::column_type::public_input), var(0, 7, false, var::column_type::public_input)};

    typename component_type::params_type params = {input_var_a, input_var_b};

    std::vector<typename BlueprintFieldType::value_type> public_input = {
        0xc801afd_cppui255, 0xc801afd_cppui255, 0xc801afd_cppui255, 0xc801afd_cppui255,
        0xc801afd_cppui255, 0xc801afd_cppui255, 0xc801afd_cppui255, 0xc801afd_cppui255};
    // std::vector<typename BlueprintFieldType::value_type> public_input = {1, 0, 0, 0, 1, 0, 0, 0};

    auto result_check = [](AssignmentType &assignment, component_type::result_type &real_res) {
        std::cout << "multiplication ress : " << assignment.var_value(real_res.output[0]).data << " "
                  << assignment.var_value(real_res.output[1]).data << " "
                  << assignment.var_value(real_res.output[2]).data << " "
                  << assignment.var_value(real_res.output[3]).data << " " << std::endl;
    };

    using ArithmetizationType = zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>;
    using component_type = component_type;

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

    assignment_bp.padding();
    zk::snark::plonk_assignment_table<BlueprintFieldType, ArithmetizationParams> assignments(private_assignment,
                                                                                             public_assignment);

    std::ofstream out;
    std::ofstream out_input;
    out.open("field_mul.json");
    out_input.open("field_mul_public_input.json");
    placeholder_proof_gen::serialize(out, desc, public_assignment, bp);
    boost::json::value jv = {
        {"public_input", public_input},
    };
    placeholder_proof_gen::pretty_print(out_input, jv);
}