//---------------------------------------------------------------------------//
// Copyright (c) 2018-2021 Mikhail Komarov <nemo@nil.foundation>
// Copyright (c) 2022 Aleksei Moskvin <alalmoskvin@nil.foundation>
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

#include <iostream>
#include <chrono>

#define BOOST_APPLICATION_FEATURE_NS_SELECT_BOOST

#include <boost/application.hpp>

#undef B0

#include <boost/random.hpp>
#include <boost/random/random_device.hpp>
#include <boost/json/src.hpp>
#include <boost/optional.hpp>
#include <boost/program_options/parsers.hpp>

#include <nil/state-proof-gen/aspects/args.hpp>
#include <nil/state-proof-gen/aspects/path.hpp>
#include <nil/state-proof-gen/aspects/configuration.hpp>
#include <nil/state-proof-gen/aspects/proof.hpp>
#include <nil/state-proof-gen/detail/configurable.hpp>

#include <nil/crypto3/codec/algorithm/decode.hpp>
#include <nil/crypto3/codec/base.hpp>

#include <nil/crypto3/pubkey/eddsa.hpp>

#include <nil/crypto3/algebra/curves/pallas.hpp>
#include <nil/crypto3/algebra/fields/arithmetic_params/pallas.hpp>
#include <nil/crypto3/algebra/random_element.hpp>

#include <nil/crypto3/algebra/curves/ed25519.hpp>
#include <nil/crypto3/algebra/fields/arithmetic_params/ed25519.hpp>

#include <nil/state-proof-gen/proof_generate.hpp>
#include <nil/state-proof-gen/structs_deserialization.hpp>
#include <nil/state-proof-gen/ec_index_terms.hpp> 
#include <nil/state-proof-gen/solana.hpp> 
#include <nil/state-proof-gen/mina.hpp> 
//
#include <nil/crypto3/zk/components/algebra/curves/pasta/plonk/unified_addition.hpp>
//
#include <nil/crypto3/zk/components/non_native/algebra/fields/plonk/variable_base_multiplication_edwards25519.hpp>
#include <nil/crypto3/zk/components/algebra/curves/pasta/plonk/variable_base_scalar_mul_15_wires.hpp>
//
#include <nil/crypto3/zk/assignment/plonk.hpp>
#include <nil/crypto3/zk/components/systems/snark/plonk/kimchi/verifier_base_field.hpp>
#include <nil/crypto3/zk/components/systems/snark/plonk/kimchi/verify_scalar.hpp>
#include <nil/crypto3/zk/components/systems/snark/plonk/kimchi/proof_system/circuit_description.hpp>

#include <nil/crypto3/math/algorithms/calculate_domain_set.hpp>

#include <nil/crypto3/hash/algorithm/hash.hpp>
#include <nil/crypto3/hash/keccak.hpp>
#include <nil/crypto3/hash/sha2.hpp>

#include <nil/crypto3/pubkey/algorithm/sign.hpp>
#include <nil/crypto3/pubkey/eddsa.hpp>

#include <nil/crypto3/zk/commitments/type_traits.hpp>
#include <nil/crypto3/zk/snark/arithmetization/plonk/params.hpp>
#include <nil/crypto3/zk/snark/systems/plonk/placeholder/preprocessor.hpp>
#include <nil/crypto3/zk/snark/systems/plonk/placeholder/prover.hpp>
#include <nil/crypto3/zk/snark/systems/plonk/placeholder/verifier.hpp>
#include <nil/crypto3/zk/snark/systems/plonk/placeholder/params.hpp>
//

using namespace nil::crypto3;

template<typename F, typename First, typename... Rest>
inline void insert_aspect(F f, First first, Rest... rest) {
    f(first);
    insert_aspect(f, rest...);
}

template<typename F>
inline void insert_aspect(F f) {
}

template<typename Application, typename... Aspects>
inline bool insert_aspects(boost::application::context &ctx, Application &app, Aspects... args) {
    insert_aspect([&](auto aspect) { ctx.insert<typename decltype(aspect)::element_type>(aspect); }, args...);

    boost::shared_ptr<nil::proof::aspects::path> path_aspect = boost::make_shared<nil::proof::aspects::path>();

    ctx.insert<nil::proof::aspects::path>(path_aspect);
    ctx.insert<nil::proof::aspects::configuration>(boost::make_shared<nil::proof::aspects::configuration>(path_aspect));
    ctx.insert<nil::proof::aspects::proof>(boost::make_shared<nil::proof::aspects::proof>(path_aspect));

    return true;
}

template<typename Application>
inline bool configure_aspects(boost::application::context &ctx, Application &app) {
    typedef nil::proof::detail::configurable<nil::dbms::plugin::variables_map,
                                             nil::dbms::plugin::cli_options_description,
                                             nil::dbms::plugin::cfg_options_description>
        configurable_aspect_type;

    boost::strict_lock<boost::application::aspect_map> guard(ctx);
    boost::shared_ptr<nil::proof::aspects::args> args = ctx.find<nil::proof::aspects::args>(guard);
    boost::shared_ptr<nil::proof::aspects::configuration> cfg = ctx.find<nil::proof::aspects::configuration>(guard);

    for (boost::shared_ptr<void> itr : ctx) {
        boost::static_pointer_cast<configurable_aspect_type>(itr)->set_options(cfg->cli());
        boost::static_pointer_cast<configurable_aspect_type>(itr)->set_options(cfg->cfg());
    }

    try {
        boost::program_options::store(
            boost::program_options::parse_command_line(args->argc(), args->argv(), cfg->cli()), cfg->vm());
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
    }

    for (boost::shared_ptr<void> itr : ctx) {
        boost::static_pointer_cast<configurable_aspect_type>(itr)->initialize(cfg->vm());
    }

    return false;
}

void proof_new(boost::json::value jv, boost::json::value jv_public_input, std::string output_file) {
    std::size_t proof_number = boost::json::value_to<std::size_t>(jv.at("id"));
    if (proof_number == 0) {
        solana_state_proof::proof_new(jv, jv_public_input, output_file);
    }
    if (proof_number == 1) {
        mina_state_proof::proof_new(jv, jv_public_input, output_file);
    }
}

struct prover {
    typedef nil::crypto3::hashes::sha2<256> hash_type;
    typedef nil::crypto3::algebra::curves::ed25519 signature_curve_type;
    typedef typename signature_curve_type::template g1_type<> group_type;
    typedef nil::crypto3::pubkey::eddsa<group_type, nil::crypto3::pubkey::eddsa_type::basic, void> signature_scheme_type;
    typedef typename nil::crypto3::pubkey::public_key<signature_scheme_type>::signature_type signature_type;

    prover(boost::application::context &context) : context_(context) {
    }

    int operator()() {
        BOOST_APPLICATION_FEATURE_SELECT
        std::string json_st = context_.find<nil::proof::aspects::proof>()->input_circuit_string();
        std::string json_public_input = context_.find<nil::proof::aspects::proof>()->input_public_params_string();
        std::string output_file = context_.find<nil::proof::aspects::proof>()->output_file_string();
        boost::json::error_code ec;

        boost::json::value jv = boost::json::parse( json_st, ec );
        if( ec )
            std::cout << "Json parsing failed: " << ec.message() << "\n";
        boost::json::value jv_public_input = boost::json::parse( json_public_input, ec );
        if( ec )
            std::cout << "Json public_input parsing failed: " << ec.message() << "\n";

        proof_new(jv, jv_public_input, output_file);

        return 0;
    }

    boost::application::context &context_;
};

bool setup(boost::application::context &context) {
    return false;
}

int main(int argc, char *argv[]) {
    boost::system::error_code ec;
    /*<<Create a global context application aspect pool>>*/
    boost::application::context ctx;

    boost::application::auto_handler<prover> app(ctx);

    if (!insert_aspects(ctx, app, boost::make_shared<nil::proof::aspects::args>(argc, argv))) {
        std::cout << "[E] Application aspects configuration failed!" << std::endl;
        return 1;
    }
    if (configure_aspects(ctx, app)) {
        std::cout << "[I] Setup changed the current configuration." << std::endl;
    }
    // my server instantiation
    int result = boost::application::launch<boost::application::common>(app, ctx, ec);

    if (ec) {
        std::cout << "[E] " << ec.message() << " <" << ec.value() << "> " << std::endl;
    }

    return result;
}