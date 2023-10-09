# Placeholder verifier 

[Placeholder proof system](https://nil.foundation/blog/post/placeholder-proofsystem)
is `=nil;` Foundation's proof system with IVC and custom gates, designed specifically for
in-EVM verification.
It's a zero-knowledge succinct non-interactive argument of knowledge (zkSNARK)
based on PlonK-style arithmetization.
Its modularity allows to recompile the proof system setup for reusing in different applications
with different security assumptions.

You can check its implementation in the [GitHub repo](https://github.com/nilfoundation/crypto3).

[Placeholder verifier](https://github.com/alexandra-mara/evm-placeholder-verification)
is an application for in-EVM validation of zero-knowledge proofs
generated with the Placeholder proof system.
It's a smart contract deployed on [Sepolia](https://sepolia.etherscan.io/address/0x489dbc0762b3d9bd9843db11eecd2a177d84ba2b)
and it can be deployed on your local network or any test or main net.

## Installation
### Installing verifier as a dependency

You can install the package via `npm` from the command line:

```bash
npm install @nilfoundation/evm-placeholder-verification@1.1.1
```

or add it to the `package.json` file manually:

```json
"@nilfoundation/evm-placeholder-verification": "1.1.1"
```

### Building from sources

Clone the project from GitHub:

```bash
git clone git@github.com:NilFoundation/evm-placeholder-verification.git
```

After that, navigate to the `evm-placeholder-verification` directory:

```bash
cd evm-placeholder-verification
```

Install the dependency packages:

```bash
npm i
```

## Usage

You can call the verifier contract and use it on chain by importing it like this:

```
import '@nilfoundation/evm-placeholder-verification/contracts/verifier.sol';
```

Example of contracts using the verifier:
https://github.com/NilFoundation/evm-proof-market/blob/master/contracts/verifiers/unified_addition_verifier.sol
Mina example https://github.com/NilFoundation/mina-state-proof/blob/master/contracts/state_proof/mina_state_proof.sol

## Compile contracts

```bash
npx hardhat compile
```

## Deploy

To launch a local network using the following command:

```bash
npx hardhat node
```

Don't close the terminal and don't finish this process, the Hardhat node should be
running for the next steps.

To deploy to a test environment (Ganache, for example), run the following
from another terminal:

```bash
npx hardhat deploy --network localhost
```

Hardhat reuses old deployments by default; to force re-deploy,
add the `--reset` flag to the command.

If you want to deploy to Sepolia or any other live net, follow
the [Hardhat's instructions](https://hardhat.org/tutorial/deploying-to-a-live-network).

### Testing

Tests are located in the `test` directory.
To run tests:

```bash
npx hardhat test # Execute tests
REPORT_GAS=true npx hardhat test # Test with gas reporting
```

## Local verification of zkLLVM circuit compiler output

[zkLLVM compiler](https://github.com/NilFoundation/zkllvm) prepares circuits
as instantiated contracts that can be deployed to a blockchain.

Once you get zkLLVM output, create a circuit directory under `contracts/zkllvm` for your output.
That directory should contain the following files:

```
* proof.bin — Placeholder proof file
* circuit_params.json — parameters file
* public_input.json — file with public input
* linked_libs_list.json — list of external libraries that have to be deployed for gate argument computation
* gate_argument.sol, gate0.sol, ... gateN.sol — Solidity files with gate argument computation
```

If all these files are in place, you can deploy the verifier app and verify the proofs.
You only need to deploy the verifier once, and then you can verify as many proofs as you want.

Deploying the contracts:

```bash
npx hardhat deploy
```

If you've put the files under, let's say, `contracts/zkllvm/circuit-name` directory,
you can verify the proofs with the following:

```bash
npx hardhat verify-circuit-proof --test circuit-name
```

To verify all circuits from `contracts/zkllvm` directory, run:

```bash
npx hardhat verify-circuit-proof-all
```
