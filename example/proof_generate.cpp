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

#include <nil/crypto3/algebra/curves/pallas.hpp>
#include <nil/crypto3/algebra/fields/arithmetic_params/pallas.hpp>

#include <nil/crypto3/hash/keccak.hpp>

#include <nil/crypto3/zk/snark/arithmetization/plonk/params.hpp>

#include <nil/crypto3/zk/blueprint/plonk.hpp>
#include <nil/crypto3/zk/assignment/plonk.hpp>
#include <nil/crypto3/zk/components/algebra/curves/pasta/plonk/unified_addition.hpp>

#include "proof_generate.hpp"

#include <nil/crypto3/zk/snark/arithmetization/plonk/table_description.hpp>

#include <nil/crypto3/placeholder-proof-gen/json_serialization.hpp>
#include <nil/crypto3/placeholder-proof-gen/json_deserialization.hpp>

#include <nil/crypto3/zk/components/non_native/algebra/fields/plonk/ed25519.hpp>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

using namespace nil::crypto3;

int main() {
        std::string json_st, json_public_input;
        std::string FILENAME_CIRCUIT = "/Users/zerg/Projects/placeholder-proof-gen/example/data/unified_addition.json";
        std::string FILENAME_PUBLIC_PARAMS = "/Users/zerg/Projects/placeholder-proof-gen/example/data/unified_addition_public_params.json";
        boost::json::error_code ec;
        if (boost::filesystem::exists(FILENAME_CIRCUIT)) {
            boost::filesystem::load_string_file(FILENAME_CIRCUIT, json_st);
        }
        boost::json::value jv = boost::json::parse( json_st, ec );
        if( ec )
            std::cout << "Json parsing failed: " << ec.message() << "\n";

        if (boost::filesystem::exists(FILENAME_PUBLIC_PARAMS)) {
            boost::filesystem::load_string_file(FILENAME_PUBLIC_PARAMS, json_public_input);
        }
        boost::json::value jv_public_input = boost::json::parse( json_public_input, ec );
        if( ec )
            std::cout << "Json public_input parsing failed: " << ec.message() << "\n";

//        if (arithmetization_params == placeholder_proof_gen::arithmetization_params_t(11, 1, 1, 1)) {  // unified_addition
            using curve_type = algebra::curves::pallas;
            using BlueprintFieldType = typename curve_type::base_field_type;
            using hash_type = nil::crypto3::hashes::keccak_1600<256>;
            using var = zk::snark::plonk_variable<BlueprintFieldType>;

            constexpr std::size_t WitnessColumns = 11;
            constexpr std::size_t PublicInputColumns = 1;
            constexpr std::size_t ConstantColumns = 1;
            constexpr std::size_t SelectorColumns = 1;
            using ArithmetizationParams =
                    zk::snark::plonk_arithmetization_params<WitnessColumns, PublicInputColumns, ConstantColumns, SelectorColumns>;
            using ArithmetizationType = zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>;
            using AssignmentType = zk::blueprint_assignment_table<ArithmetizationType>;

            constexpr std::size_t Lambda = 40;

            zk::snark::plonk_table_description<BlueprintFieldType, ArithmetizationParams> desc = boost::json::value_to<zk::snark::plonk_table_description<BlueprintFieldType, ArithmetizationParams>>(jv.at("desc"));
            zk::blueprint_public_assignment_table<ArithmetizationType> public_assignment = zk::generate_tmp(jv.at("public_assignment"), desc);
            zk::blueprint<ArithmetizationType> bp(boost::json::value_to<zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>>(jv.at("bp")), desc);
            std::vector<typename BlueprintFieldType::value_type> public_input = boost::json::value_to<std::vector<typename BlueprintFieldType::value_type>>(jv_public_input.at("public_input"));

            using component_type = zk::components::curve_element_unified_addition<ArithmetizationType, curve_type, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10>;
            typename component_type::params_type params = {
                    {var(0, 0, false, var::column_type::public_input), var(0, 1, false, var::column_type::public_input)},
                    {var(0, 2, false, var::column_type::public_input), var(0, 3, false, var::column_type::public_input)}};


            proof_generate<component_type, BlueprintFieldType, ArithmetizationParams, hash_type, Lambda>(params, public_input, desc, bp, public_assignment);
//        }

}