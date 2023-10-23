# Placeholder verifier

Placeholder proof system and its associated verifier offer an efficient solution
for zero-knowledge proof verification.
Whether you are building complex decentralized applications or need to verify
the correctness of computations in a trustless manner, Placeholder provides
flexibility and modularity to meet your specific requirements.
This documentation serves as a guide to help you make the most of this powerful tool.

## What is Placeholder

[Placeholder proof system](https://nil.foundation/blog/post/placeholder-proofsystem)
is a cutting-edge proof system with IVC (Incrementally Verifiable Computations)
and custom gates.
It's built to enable zero-knowledge functionality and secure interactions
on the Ethereum blockchain and other blockchain networks.
Placeholder serves a zero-knowledge succinct non-interactive argument of knowledge
(zkSNARK) based on PlonK-style arithmetization.
Its modularity allows configuring and adapting it for various use cases,
each with distinct security requirements.

For technical details and implementation insights, you can explore the source code
in [Crypto3's GitHub repository](https://github.com/nilfoundation/crypto3).

## What is Placeholder verifier

[Placeholder verifier](https://github.com/NilFoundation/evm-placeholder-verification/)
is an essential component that enables the in-EVM verification of zero-knowledge proofs
generated using the Placeholder proof system.
It's a smart contract application that can be deployed on different Ethereum networks,
including local, test, and main networks.

Currently, it's deployed on the Sepolia network
([contract](https://sepolia.etherscan.io/address/0x489dbc0762b3d9bd9843db11eecd2a177d84ba2b)).
You can check the full code and tests
[on GitHub](https://github.com/NilFoundation/evm-placeholder-verification/).

## Installing verifier as a dependency

The verifier is published as an npm package, so you can easily add it as a dependency
for your project.
Open a terminal and run the following command from your project's home:

```bash
npm install @nilfoundation/evm-placeholder-verification@1.1.1
```

Alternatively, you can manually add it to your project's `package.json` file
by including the following line:

```json
"@nilfoundation/evm-placeholder-verification": "1.1.1"
```

## On-chain verification

For on-chain verification of zero-knowledge proofs via Placeholder verifier
import the verifier contract in your Solidity code as shown below:

```
import '@nilfoundation/evm-placeholder-verification/contracts/verifier.sol';
```

To use the verifier contract, you're going to need the following parameters:
* address for the deployed verifier contracts;
* address for the deployed gate arguments.

For examples of contracts that utilize the verifier, you can refer
to the following repositories:
[Basic example of a verifier](https://github.com/NilFoundation/evm-proof-market/blob/master/contracts/verifiers/unified_addition_verifier.sol);
[Mina state verification](https://github.com/NilFoundation/mina-state-proof/blob/master/contracts/state_proof/mina_state_proof.sol).

### Contract Addresses
#### PlaceholderVerifier

| Network      | Address |
| ----------- | ----------- |
| Sepolia      | [`0x489dbc0762b3d9bd9843db11eecd2a177d84ba2b`](https://sepolia.etherscan.io/address/0x489dbc0762b3d9bd9843db11eecd2a177d84ba2b)      |

#### UnifiedAdditionVerifier

[UnifiedAdditionVerifier](https://github.com/NilFoundation/evm-proof-market/blob/master/contracts/verifiers/unified_addition_verifier.sol)
is a part of [Proof Market's Endpoint](https://nil.foundation/blog/post/proofmarket-evm-endpoint).

| Network      | Address |
| ----------- | ----------- |
| Sepolia      | [`0xc6E7A6Dcad73D499520DDdf5d9b56E0E18DD9bAd`](https://sepolia.etherscan.io/address/0xc6E7A6Dcad73D499520DDdf5d9b56E0E18DD9bAd)       |

## Building from sources

You can follow these steps if you prefer to build the verifier from sources.

1. Clone the project from GitHub:

```bash
git clone git@github.com:NilFoundation/evm-placeholder-verification.git
```

2. Navigate to the `evm-placeholder-verification` directory:

```bash
cd evm-placeholder-verification
```

3. Install the necessary dependency packages:

```bash
npm i
```

4. Run the following command in your terminal to compile the verifier's contracts:

```bash
npx hardhat compile
```

5. The tests for this application are located in the `test` directory.
You can execute tests using the following commands:

```bash
npx hardhat test # Execute tests
REPORT_GAS=true npx hardhat test # Test with gas reporting
```

### Deploying the verifier
#### Local network

First, launch a Hardhat node using the following command:

```bash
npx hardhat node
```

Keep the terminal open and the network running for the subsequent deployment steps.

To deploy to a test environment (Ganache, for example), run the following command in another terminal window:

```bash
npx hardhat deploy --network localhost
```

By default, Hardhat reuses old deployments.
Add the `--reset` flag to force a fresh deployment.

#### Live networks

For deployments to live Ethereum networks like Sepolia, follow
the [Hardhat's deployment guide](https://hardhat.org/tutorial/deploying-to-a-live-network)
for specific instructions.

## Local verification of zkLLVM circuit compiler output

You can use Placeholder verifier for local verification of zero-knowledge proofs
generated by the zkLLVM compiler.
This is a powerful feature that allows you to validate proofs without relying
on external parties.

1. Follow the steps from [Building from sources section](#building-from-sources).

2. Use the [zkLLVM compiler](https://github.com/NilFoundation/zkllvm) to prepare circuits
as instantiated contracts that can be deployed to a blockchain.

3. Once you get zkLLVM output, create a circuit directory under `contracts/zkllvm`
for your output.
That directory should contain the following files:
* `proof.bin` — Placeholder proof file
* `circuit_params.json` — parameters file
* `public_input.json` — file with public input
* `linked_libs_list.json` — list of external libraries to be deployed for gate argument computation
* `gate_argument.sol, gate0.sol, ... gateN.sol` — Solidity files with gate argument computation

4. With all these files in place, you can deploy the verifier application
and start verifying proofs.
You only need to deploy the verifier once, and then you can verify as many proofs
as you need.

To deploy the verifier contracts, run:

```bash
npx hardhat deploy
```

Assuming you've placed your zkLLVM output files in a directory like
`contracts/zkllvm/circuit-name`, you can verify the proofs with this command:

```bash
npx hardhat verify-circuit-proof --test circuit-name
```

To verify all circuits from the `contracts/zkllvm` directory, run:

```bash
npx hardhat verify-circuit-proof-all
```
